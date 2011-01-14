#include "neural_daq.h"
#include "trode.h"

bool acquiring;
int master_id;
int buffer_count = 0;
int32 buffer_size;
std::map <int, neural_daq> neural_daq_map;

void neural_daq_init(boost::property_tree::ptree &setup_pt){

  acquiring = false;
  master_id = 0; // arbitrarily pick a card to be master. maybe later user can choose this through conf file?
  buffer_count = 0;

  // temp vars for settings import
  neural_daq this_neural_daq;
  boost::property_tree::ptree ndaq_pt;

  // temp vars for nidaqmx initialization
  int samp_rate = (int)1e6/32;  // this is the card's max: 31250 Hz per channel, 32 channels
  int buffer_samps_per_chan = 32; // hard-coded for now, input data chunk width: 32 samples
  //int buffer_size;
  int32 daqErr = 0;
  char clk_src[256], channel_name[256], trig_name[256]; // strings to be set by nidaqmx fn's
  float64 clkRate;
  neural_daq this_nd;
  neural_daq master_daq;
  int n_completed = 0;
  bool master_completed = false;

  // import the settings
  BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		setup_pt.get_child("options.setup.neural_daq_list")){
    // bring in properties from the config file
    ndaq_pt= v.second;
    assign_property <int> ("id", &(this_nd.id), ndaq_pt, ndaq_pt, 1);
    assign_property <int> ("n_samps_per_buffer", &(this_nd.n_samps_per_buffer), ndaq_pt, ndaq_pt, 1);
    assign_property <int> ("n_chans", &(this_nd.n_chans), ndaq_pt, ndaq_pt, 1);
    assign_property <std::string> ("dev_name", &(this_nd.dev_name), ndaq_pt, ndaq_pt, 1);
    assign_property <std::string> ("in_filename", &(this_nd.in_filename), ndaq_pt, ndaq_pt, 1);
    assign_property <std::string> ("raw_dump_filename", &(this_nd.raw_dump_filename), ndaq_pt, ndaq_pt, 1);

    // set up a buffer to use as this daq's input stream
    this_nd.data_ptr = new float64 [this_nd.n_chans * this_nd.n_samps_per_buffer];
    init_array <float64>(this_nd.data_ptr, 0.0, (this_nd.n_chans * this_nd.n_samps_per_buffer) );
    neural_daq_map.insert( std::pair<int,neural_daq> (this_nd.id, this_nd) );
  }

  int n_daq = neural_daq_map.size();
  int n = master_id;
  std::map<int, neural_daq>::iterator it;
  while(n_completed < n_daq){
    if(!master_completed){   // we want to init the master card first
      n = master_id;         // at end of that init, set n to 0,
    } else{                  // then iterate through the other cards
      if (n == master_id){
	n++;}
    }
    it = neural_daq_map.find(n);
    this_nd = (*it).second;

    this_nd.task_handle = 0; // dunno why, but most examples do this 0 init
    buffer_size = buffer_samps_per_chan * this_nd.n_chans;
    daq_err_check( (DAQmxCreateTask(this_nd.dev_name.c_str(), &(this_nd.task_handle))) );
    for (int c = 0; c < this_nd.n_chans; c++){
      sprintf(channel_name, "%s/ai%d", this_nd.dev_name.c_str(), c);
      daq_err_check ( DAQmxCreateAIVoltageChan(this_nd.task_handle,channel_name,"",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL) );
    }

    daq_err_check ( DAQmxCfgSampClkTiming(this_nd.task_handle, "", samp_rate, DAQmx_Val_Rising, DAQmx_Val_ContSamps, buffer_size) );

    if( n == master_id ){
      std::cout << "Processing master daq" << std::endl;
      master_daq = this_nd;
      daq_err_check ( DAQmxSetRefClkSrc( this_nd.task_handle, "OnboardClock") );
      if(n_daq > 1)
	daq_err_check ( DAQmxGetRefClkSrc( master_daq.task_handle, clk_src, 256) );
      daq_err_check ( DAQmxGetRefClkRate( master_daq.task_handle, &clkRate) );
      if(n_daq > 1)
	daq_err_check ( GetTerminalNameWithDevPrefix(master_daq.task_handle, "ai/StartTrigger", trig_name) );
      daq_err_check ( DAQmxRegisterEveryNSamplesEvent( master_daq.task_handle, DAQmx_Val_Acquired_Into_Buffer, 32,0,EveryNCallback,(void *)&neural_daq_map) );
      daq_err_check ( DAQmxRegisterDoneEvent(master_daq.task_handle, 0, DoneCallback, (void *)&master_daq) );

      master_completed = true;
      n = 0;
    } else {
      std::cout << "Processing a slave daq." << std::endl;
      daq_err_check ( DAQmxSetRefClkSrc( this_nd.task_handle, clk_src) );
      daq_err_check ( DAQmxSetRefClkRate(this_nd.task_handle, clkRate) );
      daq_err_check ( DAQmxCfgDigEdgeStartTrig( this_nd.task_handle, trig_name, DAQmx_Val_Rising) );
      // Register every n samples?  No.  we only want the master card to do this.
      daq_err_check ( DAQmxRegisterDoneEvent(this_nd.task_handle, 0, DoneCallback, (void *)&this_nd) );
    }
    this_nd.status = 0;
    neural_daq_map[this_nd.id] = this_nd; // we gained a task handle for each nd. must re-insert into the map for that value to persist
    n_completed++;
  }
}

void neural_daq_start_all(void){
  
  std::cout << "Print from start_all" << std::endl;
  std::map<std::string, Trode>::iterator a;
  for(a = trode_map.begin(); a != trode_map.end(); a++){
    (*a).second.print_options();
  }
  std::cout << "Finished print from start_all" << std::endl;
  fflush(stdout);

  // start all slave tasks first, so that they don't miss the start trigger from the master
  std::map<int, neural_daq>::iterator it;
  buffer_count = 0;
  for(int n = 0; n < neural_daq_map.size(); n++){
    if(n != master_id){
      daq_err_check ( DAQmxStartTask( (neural_daq_map.find(n)->second).task_handle ) );
      it = neural_daq_map.find(n);
      neural_daq nd;
      nd = (*it).second;
      nd.status= 1;
      neural_daq_map[nd.id] = nd;
    }
  }
  // then start the master task
  acquiring = true;
  daq_err_check ( DAQmxStartTask( neural_daq_map.find(master_id)->second.task_handle) );
}

void neural_daq_stop_all(void){
  acquiring = false;
  //sleep(1);  // why sleep?  b/c for some reason hitting immediately after running program hangs the computer (threads' fault?)
  std::map<int, neural_daq>::iterator it;
  
  bool32 isDone;
  for(it = neural_daq_map.begin(); it != neural_daq_map.end(); it++){
    std::cout << "About to try to stop task_handle: " << (*it).second.task_handle << " on dev: " << (*it).second.dev_name << std::endl;
    daq_err_check ( DAQmxIsTaskDone((*it).second.task_handle, &isDone) );
    std::cout << "In stop task loop.  Task handle " << (*it).second.task_handle << " is done: " << isDone << std::endl;
    daq_err_check ( DAQmxStopTask( (*it).second.task_handle ) );


    daq_err_check ( DAQmxIsTaskDone((*it).second.task_handle, &isDone) );
    std::cout << "In stop task loop.  Task handle: " << (*it).second.task_handle << " is done: " << isDone << std::endl;
    std::cout << "About to clear task: " << (*it).second.task_handle << std::endl;

    daq_err_check ( DAQmxClearTask( (*it).second.task_handle) );

  }
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNSamplesEventType, uInt32 nSamples, void *callbackData){
  if(acquiring){
    std::cout << "First EveryNCallback" << std::endl; 
   //buffer_count = 0;
    buffer_count++;
    int32 read;
    printf("%d\r",buffer_count);
    fflush(stdout); // cause I wanna see the number grawing FAST ^^
    for(std::map<int,neural_daq>::iterator it = neural_daq_map.begin(); it != neural_daq_map.end(); it++){
      daq_err_check ( DAQmxReadAnalogF64( (*it).second.task_handle, 32, 10.0, DAQmx_Val_GroupByChannel, (*it).second.data_ptr, buffer_size, &read,NULL) );
    }
    Trode tt01;
    Trode tt02;
    Trode tt03;
    Trode tt04;
    std::cout << "trode_map.size(): " << trode_map.size() << std::endl;
    std::map<std::string, Trode>::iterator tmp = trode_map.begin();
    for(std::map<std::string, Trode>::iterator it = trode_map.begin(); it != trode_map.end(); it++){
      Trode this_trode = (*it).second;
      Trode next_trode = (*(it++)).second;
      tt01 = trode_map["tt01"];      
      tt02 = trode_map["tt02"];
      tt03 = trode_map["tt03"];
      tt04 = trode_map["tt04"];
      //this_trode.print_options();
      //fflush(stdout);
      trode_filter_data(&this_trode);
      //it = trode_map.find("tt03");
      //std::cout << "trode_map.size(): " << trode_map.size() << std::endl;
      //it++;
     }
    //this_trode.print_options();
  }
}

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData){
  std::cout << "DoneCallback Called." << std::endl;
  neural_daq nd = find_neural_daq_by_taskhandle(taskHandle);
  print_neural_daq(nd);
  fflush(stdout);
}

void print_neural_daq(neural_daq nd){
  std::cout << "id: " << nd.id << std::endl << "dev_name: " << nd.dev_name <<
    "status: " << nd.status << std::endl;
}

neural_daq find_neural_daq_by_taskhandle(TaskHandle taskhandle){
  std::map<int, neural_daq>::iterator it;
  for(it = neural_daq_map.begin(); it != neural_daq_map.end(); it++){
    if( (*it).second.task_handle = taskhandle){
      return (*it).second;
    }
  }
  std::cout << "Couldn't find the taskhandle: " << taskhandle << std::endl;
}
