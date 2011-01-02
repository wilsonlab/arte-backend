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
Trode test_t;
std::map<std::string, neural_daq> neural_daq_map;
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

  //std::string t2 = "test.conf";
  //read_xml(t2, setup_pt);

  try{
    //const std::string ts= "/home/greghale/arte-ephys/conf/arte_setup_default.conf";
    //std::cout << setup_fn << std::endl;
    //read_xml(ts, setup_pt);
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

}

void arte_setup_init(int argc, char *argv[]){

  // Mostly for initializing the neural daq cards.  But other global state stuff
  // could be initialized here, too.

  BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		setup_pt.get_child("options.setup.neural_daq_list")) {};
    //neural_daq_map.push_back(new_neural_daq(v));

}


void arte_session_init(int argc, char *argv[]){

  Trode trode_default(); // store the default settings here


  BOOST_FOREACH(boost::property_tree::ptree::value_type &v, 
		session_pt.get_child("options.session.trodes"))
    trode_map.insert( std::pair<std::string, Trode> ( v.second.data(), new_trode(v)) );
		

}

Trode new_trode(boost::property_tree::ptree::value_type &v){

  std::istringstream iss; // helper istream to convert text to ints, floats, etc.
  std::string str;

  // check the uniquness of the new name
  assert(trode_map.find(v.second.data()) == trode_map.end()); // assert that finding the trode name returns map::end iterator, meaning name doesn't exist as key in list

  Trode new_trode;

  boost::property_tree::ptree this_trode_pt;
  boost::property_tree::ptree default_pt;

  this_trode_pt = v.second;
  default_pt = session_pt.get_child("options.session.trode_default");
  new_trode.trode_name = this_trode_pt.data();

  str = "n_chans";
  assign_trode_property <int> (str, &(new_trode.n_chans), this_trode_pt, default_pt,1);

  std::cout << "Trode: " << new_trode.trode_name << " has " << new_trode.n_chans << " chans." << std::endl;

  new_trode.n_chans;
  new_trode.chan_inds = new int [new_trode.n_chans];

  str = "thresholds";
  new_trode.thresholds = new float64 [new_trode.n_chans];
  assign_trode_property <float64> (str, new_trode.thresholds, this_trode_pt, default_pt, new_trode.n_chans);

  str = "channels";
  new_trode.channels = new int [new_trode.n_chans];
  assign_trode_property <int> (str, new_trode.channels, this_trode_pt, default_pt, new_trode.n_chans);

  str = "daq_id";
  assign_trode_property <int> (str, &(new_trode.daq_id), this_trode_pt, default_pt, 1);

  str = "samps_before_trig";
  assign_trode_property <int> (str, &(new_trode.samps_before_trig), this_trode_pt, default_pt, 1);

  str = "samps_after_trig";
  assign_trode_property <int> (str, &(new_trode.samps_after_trig), this_trode.pt, default_pt, 1);

  str = "spike_time";
  assign_trode_property <std::string> (str, &(new_trode.spike_mode), this_trode.pt, default_pt, 1);

  str = "win_heights";
  assign_trode_property <float64> (str, new_trode.win_heights, this_trode.pt, default_pt, new_trode.n_chans);

  

  new_trode.n_samps_per_chan = 1 + new_trode.samps_before_trig + new_trode.samps_after_trig;
  //new_trode.buffer_mult_of_input = 

  //std::cout << "Thresholds: ";
  //for(int n = 0; n< new_trode.n_chans; n++)
  //  std::cout << new_trode.thresholds[n] << " ";
  //std::cout << std::endl;

  //new_trode.buffer_mult_of_input = FIX;
  //new_trode.ptr_to_raw_stream = FIX;
  //new_trode.filt_buffer = FIX;
  //new_trode.raw_buffer = new float64 [FIX];
  //new_trode.raw_data_cursor = 0;
  //new_trode.filt_data_cursor = 0;
  //new_trode.raw_cursor_time = 0; 
  //new_trode.filt_cursor_time = 0;
  return new_trode;

}
