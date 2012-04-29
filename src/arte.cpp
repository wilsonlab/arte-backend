// this will be the arte main loop
#include "global_defs.h"
#include "arte.h"
#include "arteopt.h"
#include <iostream>
#include <boost/foreach.hpp>
#include <util.h>
#include <neural_daq.h>
#include "arte_pb.pb.h"

int main(int argc, char *argv[]){

  // implement in arteopt. setup done there for global neural_daq_map and trode_map
  arte_init(argc, argv, default_setup_config_filename, default_session_config_filename);
  
  //printf("from main, about co tall neural_daq_start_all()\n");
  //neural_daq_start_all(); // now gets called before the timer initializes
  //this begins calls to EveryNSamplesCallback, however since
  // our 'acquiring' flag is still set to false, the buffers are not
  // processed, until the return from timer.init2();
  // (this happens in src/arteopt.cpp
  //neural_daq_start_all();
  //arte_timer.start();

  std::cout << "Hit return to stop acquisition." << std::endl;
  getchar();
  neural_daq_stop_all();
  try_fclose( main_file );

  std::cout << "After call to neural_daq_stop_all()" << std::endl;
 
  return 0;
}
