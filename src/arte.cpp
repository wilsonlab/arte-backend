// this will be the arte main loop
#include "global_defs.h"
#include "arte.h"
#include "arteopt.h"
#include <iostream>
#include <boost/foreach.hpp>
#include "util.h"
#include "neural_daq.h"

int main(int argc, char *argv[]){

  // implement in arteopt. setup done there for global neural_daq_map and trode_map
  arte_init(argc, argv, default_setup_config_filename, default_session_config_filename);

  neural_daq_start_all();
  std::cout << "Hit return to stop acquisition." << std::endl;
  getchar();
  neural_daq_stop_all();
  std::cout << "After call to neural_daq_stop_all()" << std::endl;
 
  return 0;
}
