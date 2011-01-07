#include "arteopt.h"
#include "timer.h"
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <assert.h>
#include "global_defs.h"
#include <iostream>
#include "util.h"

//using namespace boost::property_tree;

std::string setup_config_filename;
std::string session_config_filename;
std::map<std::string, Trode> trode_map;
std::map<int, neural_daq> neural_daq_map;
std::map<std::string, Filt> filt_map;
boost::property_tree::ptree setup_pt;
boost::property_tree::ptree session_pt;

std::vector<TaskHandle> task_handle_vector;

void arte_init(int argc, char *argv[], const std::string &setup_fn, const std::string &session_fn){

  if(!setup_fn.empty())
    setup_config_filename = setup_fn.data();
  else
    setup_config_filename = default_setup_config_filename.data();  
  if(!session_fn.empty())
    session_config_filename = session_fn;    
  else
    session_config_filename = session_fn;

  try{
    read_xml(setup_config_filename,   setup_pt,   boost::property_tree::xml_parser::trim_whitespace); // check where this flag actually lives
    read_xml(session_config_filename, session_pt, boost::property_tree::xml_parser::trim_whitespace); // can/should put 2 possible fails in one try block?
  }
  catch(...){  // find out where the xml_parse_error lives, & how to handle it
    std::cout << "XML read error was thrown - from arteopt.cpp" << std::endl;
  }

  arte_setup_init(argc, argv); // Use the property_tree to set global vars
  arte_session_init(argc, argv); // Use property_tree to set up trode list, trode/eeg view vars
  //arte_init_timer();  // in timer.h
  //arte_start_clock(); // in timer.h 
  //arte_setup_daq_cards(); // No args. Setup task, virtual chans for trodes, callbacks.

  // after this, the main loop will start the gui.
  // EDIT:  after this, we'll sleep, waiting for n_samps callback from the cards or start/stop/configure commands from the tubes!
  // 
 
  //  std::pair<std::string, Trode> it;
  //BOOST_FOREACH(it, trode_map){
  //  it.second.print_options();
  //}
}

void arte_setup_init(int argc, char *argv[]){

  // Mostly for initializing the neural daq cards.  But other global state stuff
  // could be initialized here, too.

  BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		setup_pt.get_child("options.setup.neural_daq_list")){
    neural_daq this_neural_daq;
    boost::property_tree::ptree ndaq_pt;
    ndaq_pt = v.second;
    assign_property <int> ("id", &(this_neural_daq.id), ndaq_pt, ndaq_pt, 1);
    assign_property <int> ("n_samps_per_buffer", &(this_neural_daq.n_samps_per_buffer), ndaq_pt, ndaq_pt, 1);
    assign_property <int> ("n_chans", &(this_neural_daq.n_chans), ndaq_pt, ndaq_pt, 1);
    assign_property <std::string> ("dev_name", &(this_neural_daq.dev_name), ndaq_pt, ndaq_pt, 1);
    assign_property <std::string> ("in_filename", &(this_neural_daq.in_filename), ndaq_pt, ndaq_pt, 1);
    assign_property <std::string> ("raw_dump_filename", &(this_neural_daq.raw_dump_filename), ndaq_pt, ndaq_pt, 1);
    
    // setup an array to use as the input stream from this daq card
    this_neural_daq.data_ptr = new float64 [ this_neural_daq.n_chans * this_neural_daq.n_samps_per_buffer ];
    init_array<float64>(this_neural_daq.data_ptr,0.0, this_neural_daq.n_chans * this_neural_daq.n_samps_per_buffer); 
    neural_daq_map.insert( std::pair <int, neural_daq> (this_neural_daq.id, this_neural_daq));
  }

  BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		setup_pt.get_child("options.setup.filter_list")){
    Filt this_filt;
    boost::property_tree::ptree filt_pt;
    filt_pt = v.second;
    this_filt.filt_name = filt_pt.data();
    assign_property <std::string> ("type", &(this_filt.type), filt_pt, filt_pt, 1);
    assign_property <int> ("order", &(this_filt.order), filt_pt, filt_pt, 1);
    assign_property <bool> ("filtfilt", &(this_filt.filtfilt),filt_pt,filt_pt,1);
    assign_property <int> ("filtfilt_wait_n_buffers", &(this_filt.filtfilt_wait_n_buffers), filt_pt, filt_pt,1);
    this_filt.data_cursor = 0;
    // derive properties for fir filter if fir
    if( this_filt.type.compare("fir") == 0){
      this_filt.num_coefs = new float64 [this_filt.order];
      this_filt.denom_coefs = new float64[this_filt.order];
    } // or for iir filtern
    else if( (this_filt.type.compare("iir") == 0) ){
      this_filt.filt_num_sos = this_filt.order / 2;
      int n_coefs = this_filt.order * 3;
      this_filt.num_coefs = new float64 [n_coefs];
      this_filt.denom_coefs = new float64 [n_coefs];
    } else{
      std::cerr << "Can't use the filt type: " << this_filt.type << std::endl;
    }
    filt_map.insert( std::pair<std::string, Filt>(this_filt.filt_name, this_filt) );
  }
}


void arte_session_init(int argc, char *argv[]){

  Trode trode_default(); // store the default settings here
  boost::property_tree::ptree default_trode_pt;
  boost::property_tree::ptree this_trode_pt;

  default_trode_pt = session_pt.get_child("options.session.trode_default");

  BOOST_FOREACH(boost::property_tree::ptree::value_type &v, 
		session_pt.get_child("options.session.trodes")){
    Trode this_trode;
    this_trode_pt = v.second;
    //init_new_trode(v, this_trode);
    this_trode.init(this_trode_pt, default_trode_pt, neural_daq_map, filt_map);
    trode_map.insert( std::pair<std::string, Trode> ( v.second.data(), this_trode ));
  }

}



int arte_setup_daq_cards(){
  int32 daqErr = 0;
  //TaskHandle masterTaskHandle = 0;
  //TaskHandle slaveTaskHandle = 0;
  masterTaskHandle = 0;
  daqErr = DAQmxCreateTask("",&masterTaskHandle);
}
