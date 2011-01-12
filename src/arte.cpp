// this will be the arte main loop
#include "arte.h"
#include "arteopt.h"
#include <iostream>
#include "global_defs.h"
#include <boost/foreach.hpp>
#include "util.h"
#include "neural_daq.h"

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
  neural_daq_start_all();
  std::cout << "Hit return to stop acquisition." << std::endl;
  getchar();
  neural_daq_stop_all();
  std::cout << "After call to neural_daq_stop_all()" << std::endl;
  //std::map<int, neural_daq>::iterator w;
  //for(w = neural_daq_map.begin(); w != neural_daq_map.end(); w++){
  //  std::cout << "before stopping, task handle " << (*w).second.id << " is: " << (*w).second.task_handle << std::endl;
  //  DAQmxStopTask( (*w).second.task_handle );
  //  DAQmxClearTask((*w).second.task_handle );
    //daq_err_check ( DAQmxStopTask((*w).second.task_handle)  );
    //daq_err_check ( DAQmxClearTask((*w).second.task_handle) );
    //daq_err_check
  //}
  return 0;
}
