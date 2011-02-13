#include "arteopt.h"
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <assert.h>
#include "global_defs.h"
#include <iostream>
#include "util.h"

std::string setup_config_filename;
std::string session_config_filename;
std::map<std::string, Trode> trode_map;
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
  
}

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
    trode_map.insert( std::pair<std::string, Trode> ( v.second.data(), this_trode ));
  }
  //  std::cout << "Printing from session_init" << std::endl;
  //for(std::map<std::string, Trode>::iterator it = trode_map.begin(); it != trode_map.end(); it++){
  //  (*it).second.print_options();
  //}
  std::cout << "Done session init." << std::endl;
  
  //std::map<std::string, Trode>::iterator it;
  //for(it = trode_map.begin(); it != trode_map.end(); it++){
  //  std::cout << "Trode " << (*it).second.trode_opt.trode_name << " addr: " << &((*it).second) << std::endl;
  //}
  //std::cout << "sizeof(Trode): " << sizeof(Trode) << " sizeof Trode pair: " << sizeof(*it) << std::endl;

  //fflush(stdout);
}
