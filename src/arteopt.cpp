#include "arteopt.h"
#include "timer.h"
#include <boost/foreach.hpp>
#include <assert.h>

using boost::property_tree;

void arte_init(int argc, char *argv[], std::string &setup_fn, std::string &session_fn){

  if(!setup_fn.empty())
    setup_config_filename = setup_fn.copy(); // .copy() method is the right way?
  else
    setup_config_filename = default_setup_config_filename.copy();
  
  if(!session_fn.empty())
    session_config_filename = session_fn.copy();
  else
    session_config_filename = default_session_config_filename.copy();

  try{
    read_xml(setup_config_filename,   setup_pt,   boost::property_tree::ptree::trim_whitespace); // check where this flag actually lives
    read_xml(session_config_filename, session_pt, boost::property_tree::ptree::trim_whitespace); // can/should put 2 possible fails in one try block?
  }
  catch(...){  // find out where the xml_parse_error lives, & how to handle it
    std::cout << "XML read error was thrown - from arteopt.cpp" << std::endl;
  }

  arte_setup_init(argc, argv); // Use the property_tree to set global vars
  arte_session_init(argc, argv); // Use property_tree to set up trode list, trode/eeg view vars
  arte_init_timer();  // in timer.h
  arte_start_clock(); // in timer.h 
  arte_setup_daq_cards(); // No args. Setup task, virtual chans for trodes, callbacks.

  // after this, the main loop will start the gui.

}

void arte_setup_init(int argc, char *argv[]){



}

void arte_session_init(int argc, char *argv[]){

  Trode trode_default(); // store the default settings here


  BOOST_FOREACH(ptree::value_type &v, 
		session_pt.get_child("options.session.trodes"))
    trode_group.push_back(new_trode(v));
		

}

Trode new_trode(ptree::value_type &v){

  // check the uniquness of the new name
  assert(trode_list.find(v->first) == trode_list.end()); // assert that finding the trode name returns map::end iterator, meaning name doesn't exist as key in list

  Trode new_trode;
  new_trode.trode_name = v->first;

  new_trode.n_chans = FIX;
  new_trode.chan_inds = new int [new_trode.n_chans];



  new_trode.buffer_mult_of_input = FIX;
  new_trode.ptr_to_raw_stream = FIX;
  new_trode.filt_buffer = FIX;
  new_trode.raw_buffer = new float64 [FIX];
  new_trode.raw_data_cursor = 0;
  new_trode.filt_data_cursor = 0;
  new_trode.raw_cursor_time = 0;
  new_trode.filt_cursor_time = 0;
