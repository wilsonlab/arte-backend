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
    // consider replacing this coming loop with a memset call?
    for(int m = 0; m < (this_neural_daq.n_chans * this_neural_daq.n_samps_per_buffer); m++)
      this_neural_daq.data_ptr[m] = 0.0;
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
    } // or for iir filter
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

  BOOST_FOREACH(boost::property_tree::ptree::value_type &v, 
		session_pt.get_child("options.session.trodes")){
    Trode this_trode;
    init_new_trode(v,this_trode);
    trode_map.insert( std::pair<std::string, Trode> ( v.second.data(), this_trode ));
  }

}


int init_new_trode(boost::property_tree::ptree::value_type &v, Trode &new_trode){

  std::istringstream iss; // helper istream to convert text to ints, floats, etc.
  std::string str;

  // check the uniquness of the new name
  assert(trode_map.find(v.second.data()) == trode_map.end()); // assert that finding the trode name returns map::end iterator, meaning name doesn't exist as key in list

  //Trode new_trode;

  boost::property_tree::ptree this_trode_pt;
  boost::property_tree::ptree default_pt;

  this_trode_pt = v.second;
  default_pt = session_pt.get_child("options.session.trode_default");
  new_trode.trode_name = this_trode_pt.data();
  assign_property <int> ("n_chans", &(new_trode.n_chans), this_trode_pt, default_pt,1);
  new_trode.thresholds = new float64 [new_trode.n_chans];
  assign_property <float64> ("thresholds", new_trode.thresholds, this_trode_pt, default_pt, new_trode.n_chans);
  new_trode.channels = new int [new_trode.n_chans];
  assign_property <int> ("channels", new_trode.channels, this_trode_pt, default_pt, new_trode.n_chans);
  assign_property <int> ("daq_id", &(new_trode.daq_id), this_trode_pt, default_pt, 1);
  assign_property <std::string> ("filt_name", &(new_trode.filt_name), this_trode_pt, default_pt, 1);
  assign_property <int> ("samps_before_trig", &(new_trode.samps_before_trig), this_trode_pt, default_pt, 1);
  assign_property <int> ("samps_after_trig", &(new_trode.samps_after_trig), this_trode_pt, default_pt, 1);
  assign_property <std::string> ("spike_mode", &(new_trode.spike_mode), this_trode_pt, default_pt, 1);
  new_trode.win_heights = new float64 [new_trode.n_chans];
  assign_property <float64> ("win_heights", new_trode.win_heights, this_trode_pt, default_pt, new_trode.n_chans);

  // Now the derived parts

  neural_daq my_daq = (neural_daq_map.find(new_trode.daq_id))->second;

  new_trode.n_samps_per_chan = 1 + new_trode.samps_before_trig + new_trode.samps_after_trig;
  new_trode.buffer_mult_of_input = new_trode.n_samps_per_chan / my_daq.n_samps_per_buffer;
  if((new_trode.n_samps_per_chan % my_daq.n_samps_per_buffer) >0)
    new_trode.buffer_mult_of_input += 1;

  if(filt_map.find(new_trode.filt_name) == filt_map.end())
    std::cerr << "In arteopt.cpp, didn't find filt name: " << new_trode.filt_name << std::endl;
  assert(filt_map.find(new_trode.filt_name) != filt_map.end());

  new_trode.my_filt = (filt_map.find(new_trode.filt_name))->second;

  int min_samps_for_filt = new_trode.my_filt.order;

  new_trode.my_filt.buffer_mult_of_input = min_samps_for_filt / my_daq.n_samps_per_buffer;
  if( min_samps_for_filt % my_daq.n_samps_per_buffer > 0)
    new_trode.my_filt.buffer_mult_of_input += 1;
  new_trode.my_filt.buffer_mult_of_input += new_trode.my_filt.filtfilt_wait_n_buffers;

  new_trode.my_filt.n_samps_per_chan = my_daq.n_samps_per_buffer * new_trode.my_filt.buffer_mult_of_input;

  //  new_trode.test =                new float64 [new_trode.n_chans * new_trode.my_filt.n_samps_per_chan];
  //new_trode.unfiltered_buffer =   new float64 [new_trode.n_chans * new_trode.my_filt.n_samps_per_chan];
  new_trode.filtered_buffer =     new float64 [new_trode.n_chans * new_trode.my_filt.n_samps_per_chan];
  new_trode.filtfiltered_buffer = new float64 [new_trode.n_chans * new_trode.my_filt.n_samps_per_chan];



  new_trode.ptr_to_raw_stream = my_daq.data_ptr;
  new_trode.raw_data_cursor = 0;
  new_trode.filt_data_cursor = 0;
  new_trode.raw_cursor_time = 0;
  new_trode.filt_cursor_time = 0;

  //new_trode.buffer_mult_of_input = FIX;
  //new_trode.ptr_to_raw_stream = FIX;
  //new_trode.filt_buffer = FIX;
  //new_trode.raw_buffer = new float64 [FIX];
  //new_trode.raw_data_cursor = 0;
  //new_trode.filt_data_cursor = 0;
  //new_trode.raw_cursor_time = 0; 
  //new_trode.filt_cursor_time = 0;

  // fix this to return an error code (better yet, throw exception?) if
  // there's a memory problem or some kind of illegal parameter setting
  return 0;

}
