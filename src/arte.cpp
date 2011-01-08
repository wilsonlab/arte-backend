// this will be the arte main loop
#include "arte.h"
#include "arteopt.h"
#include <iostream>
#include "global_defs.h"
#include <boost/foreach.hpp>

int main(int argc, char *argv[]){

  // implement in arteopt.  (global vars there for setup_opt, session_opt)
  arte_init(argc, argv, default_setup_config_filename, default_session_config_filename);

  // this will call gtk_main(), main loop for gui.  Nothing beyond that
  // point will be executed until the gui is closed
  
  //arte_build_gui(argc, argv);

  std::pair<std::string,Trode> v;
  std::cout << "size: " << trode_map.size() << std::endl;
  //BOOST_FOREACH(v, trode_map){
  //  v.second.print_options();
  //}
}
