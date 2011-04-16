#include "arteopt.h"
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <assert.h>
#include "global_defs.h"
#include <iostream>
#include "util.h"
#include "timer.h"

std::string setup_config_filename;
std::string session_config_filename;
std::map<uint16_t, Trode> trode_map;
std::map<std::string, Filt> filt_map;
std::map<uint16_t, Lfp_bank> lfp_bank_map;
boost::property_tree::ptree setup_pt;
boost::property_tree::ptree session_pt;
Timer arte_timer;

std::vector<TaskHandle> task_handle_vector;

void arte_init(int argc, char *argv[], const std::string &setup_fn, const std::string &session_fn){

  timestamp = 0;

  if(!setup_fn.empty())
    setup_config_filename = setup_fn.data();
  else
    setup_config_filename = default_setup_config_filename.data();  
  if(!session_fn.empty())
    session_config_filename = session_fn;    
  else
    session_config_filename = default_session_config_filename.data();

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
  
}


// Initialize the 'backplane' objects, such as the neural_daqs 
//and the pre-defined filters
void arte_setup_init(int argc, char *argv[]){

  neural_daq_init(setup_pt);

  BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		setup_pt.get_child("options.setup.filter_list")){
    Filt this_filt;
    boost::property_tree::ptree filt_pt;
    filt_pt = v.second;
    this_filt.init(filt_pt);
    filt_map.insert( std::pair<std::string, Filt>(this_filt.filt_name, this_filt) );
  }
  std::cout << "Finished arte_setup_init." << std::endl;
}


// Initialize more session-specific features, such as the 
// mapping of daq channels into tetrode groups, and 
// of daq channels into lfp_banks (collections of eegs-channels
// all coming from a signle daq card and sharing filter settings
void arte_session_init(int argc, char *argv[]){

  std::cout << "Starting session init." << std::endl;
  Trode trode_default(); // store the default settings here
  boost::property_tree::ptree default_trode_pt;
  boost::property_tree::ptree this_trode_pt;

  default_trode_pt = session_pt.get_child("options.session.trode_default");

  BOOST_FOREACH(boost::property_tree::ptree::value_type &v, 
		session_pt.get_child("options.session.trodes")){
    Trode this_trode;
    this_trode_pt = v.second;
    std::cout << "About to init a trode." << std::endl;
    this_trode.init(this_trode_pt, default_trode_pt, neural_daq_map, filt_map);
    //this_trode.print_options();
    trode_map.insert( std::pair<uint16_t, Trode> ( this_trode.trode_opt.trode_name, this_trode ));
  }

  Lfp_bank this_lfp_bank;
  BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		session_pt.get_child("options.session.lfp_banks")){
    Lfp_bank this_lfp_bank;
    boost::property_tree::ptree lfp_bank_pt;
    lfp_bank_pt = v.second;
    this_lfp_bank.init(lfp_bank_pt, neural_daq_map, filt_map);
    lfp_bank_map.insert( std::pair< uint16_t, Lfp_bank > ( this_lfp_bank.lfp_bank_name, this_lfp_bank) );
  }

  std::cout << "Done session init." << std::endl;

}
