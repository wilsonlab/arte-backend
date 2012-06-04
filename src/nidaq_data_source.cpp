#include "nidaq_data_source.h"

// Define static member
DaqList NidaqDataSource::daq_list;

// Constructor: initialize and call NiDAQmx setup functions
NidaqDataSource::NidaqDataSource(ArteNeuralDaqOptPb &daq_opt_pb,
				 shared_ptr <aTimer> timer)
  : dev_name           (daq_opt_pb.dev_name())
  , id                 (daq_opt_pb.id())
  , n_samps_per_buffer (daq_opt_pb.buffer_time_samps())
  , n_chans            (daq_opt_pb.buffer_n_chans())
  , is_master          (daq_opt_pb.is_master)
  , multiplexing       (daq_opt_pb.is_multiplexing)
  , timer              (timer)
{
  // Initialize data
  int samp_rate = (int)1e6/32;
  data = raw_voltage_array(boost::extents[n_chans][n_samps_per_buffer]);

  // Initialize daq card
  task_handle = 0;
  char taskname[50];
  sprintf( taskname, "AITask on %s", dev_name.c_str() );
  daq_err_check( (DAQmxCreateTask(taskname, &task_handle)) );
  
  if( !is_multiplexing ){
    for (int c = 0; c < n_chans; c++){
      char channel_name[50];
      sprintf( channel_name, "%s/ai%d", dev_name.c_str(), c );
      daq_err_check ( DAQmxCreateAIVoltageChan(task_handle, channel_name, "",
					       DAQmx_Val_RSE, NEURAL_DAQ_V_MIN,
					       NEURAL_DAQ_V_MAX, DAQmx_Val_Volts,
					       NULL) );
    }
    daq_err_check ( DAQmxCfgSampClkTiming(task_handle, "", samp_rate,
					  DAQmx_Val_Rising, DAQmx_Val_ContSamps,
					  n_chans * n_samps_per_buffer) );
  }
  if (is_multiplexing ){
    char channel_name[50];
    sprintf( channel_same, "%s/ai0", dev_name.c_str() );
    daq_err_check ( DAQmxCreateAIVoltageChan(task_handle, channel_name, "",
					     DAQmx_Val_RSE, NEURAL_DAQ_V_MIN,
					     NEURAL_DAQ_V_MAX, DAQmx_Val_Volts,
					     NULL) );

    daq_err_check ( DAQmxCfgSampClkTiming(task_handle, "", (samp_rate * n_chans),
					  DAQmx_Val_Rising, DAQmx_Val_ContSamps,
					  n_chans * n_samps_per_buffer) ); 
  }
  
  if( is_master ){
    daq_err_check ( DAQmxSetRefClkSrc  (task_handle, "OnboardClock") );
    daq_err_check ( DAQmxGetRefClkSrc  (task_handle, clk_src, 256) );
    daq_err_check ( DAQmxGetRefClkRate (task_handle, &clkRate) );
    daq_err_check ( GetTerminalNameWithDevPrefix(task_handle, 
						 "ai/StartTrigger", trig_name) );

    int everyN = is_multiplexing ? (n_chans * n_samps_per_buffer) : n_samps_per_buffer;
    daq_err_check ( DAQmxRegisterEveryNSamplesEvent(task_handle, 
						    DAQmx_Val_Acquired_Into_Buffer,
						    everyN, 0, EveryNCallback, 
						    (void*)&daq_list) );
  }

  if ( !is_master ){
    daq_err_check ( DAQmxSetRefClkSrc  (task_handle, clk_src) );
    daq_err_check ( DAQmxSetRefClkRate (task_handle, clkRate) );
  }

  // init the oGlom, if out_filename not empty and daq_o_glom not existing.
  // Filename will be chosen by the first daq, that file holds all daqs'
  // data.
  if ( !daq_opt_pb.out_filename() & !(daq_o_glom.use_count) ){
    daq_o_glom = new oGlom ( daq_opt_pb.out_filename(), "wb" );
  }

  // initialize the output data structure
  scratch_data.buffer_timestamp = 0;
  scratch_data.raw_voltage_array( boost::extents [n_chans][n_samps_per_buffer] );

  state = DATA_SOURCE_STOPPED;

}


void NidaqDataSource::start(){
  std::cerr << "NidaqDataSource::start() should not be called.\n";
  std::cerr << "NidaqDataSource is started as a group by ::start_all_daqs().\n";
}


void NidaqDataSource::stop(){
  std::cerr << "NidaqDataSource::stop() should not be called.\n";
  std::cerr << "NidaqDataSource is stopped as a group by ::stop_all_daqs().\n";
}


bool NidaqDataSource::get_is_master(){ return is_master }


int NidaqDataSource::get_master_daq_key( DaqList & daq_list ){
  for( DaqList::iterator it = daq_list.begin(); it != daq_list.end(); it++){
    if ( (it->second)->get_is_master() )
      return it->first;
  }
  return -1; // error code meaning no master found
}


NidaqSourcePtr NidaqDataSource::get_master_daq( DaqList & daq_list ){
  return daq_list( NidaqDataSource::get_master_daq_key( daq_list ));
}


NidaqDataSource::start_all_daqs( DaqList & daq_list ){

  NidaqSourcePtr master = get_master_daq( daq_list );
  int master_id = get_master_daq_key( daq_list );

  DaqList slave_list = daq_list;
  slave_list.erase( master_id );

  // Start slave daqs first; they need to wait for clock from master
  for( daq_list::iterator it = daq_list.begin(); it != daq_list.end(); it++ ){
    NidaqSourcePtr this_daq = it->second;
    daq_err_check ( DAQmxStartTask( this_daq->task_handle ) );
  }
  // Then starte master; everyone gets clock edges starting at the same time
  daq_err_check ( DAQmxStartTask ( master->task_handle ) );
  // The daqs will now produce data and the master will call 
  // EveryNSampsCallback once per millisecond (every 32 samples per channel)

}

    
int32 NidaqDataSource::EveryNCallback(TaskHandle task_handle,
				      int32 everyNSamplesEventType,
				      uint32 nSamples, void *callbackData){

  DaqList * daq_list = (DaqList * ) callbackData;
  NidaqSourcePtr this_daq;

  int32 read;
  int rc, n;
  long unknown_use_var;
  void *status;
  
  for( daq_list::iterator it = daq_list->begin(); it != daq_list->end(); it++){
    this_daq = it->second;
    this_daq->scratch_data.buffer_timestamp = timer->get_count() - buffer_delay;
    // interface is:
    // DAQmxReadBinaryI16(taskhandle, numSampsPerChan, timeout, fillMode,
    //                    read_array[], array_size_samps, samps_actually_ready, reserved)
    // TODO: clean this up, 
    daq_err_check(DAQmxReadBinaryI16(this_daq->task_handle, 32, 10.0, 
				     DAQmx_Val_GroupByScanNumber,
				     this_daq -> &(scratch_data_buffer.raw_voltage_array[0]),
				     buffer_size, &read, NULL));
    
    this_daq -> set_data( this_daq -> scratch_data_buffer );
  }

}


int32 NidaqDataSource::DoneCallback(TaskHandle task_handle, 
				    int32 status
				    void *callbackData){
  
}
