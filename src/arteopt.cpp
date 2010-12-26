#include "arteopt.h"

void arte_setup_init(int argc, char *argv[]);
void arte_session_init(int argc, char *argv[]); // little fn prototypes

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
  
  arte_start_clock(); // No args.  Uses global vars.		   
  arte_setup_daq_cards(); // No args. Setup task, virtual chans for trodes, callbacks.

  // after this, the main loop will start the gui.

}
