#include "arte_pb.pb.h"
#include "nidaq_data_source.h"

// Define static member
DaqList NidaqDataSource::daq_list;
float64 NidaqDataSource::clkRate = 0;
char    NidaqDataSource::clk_src[256] = "";
char    NidaqDataSource::trig_name[256] = "";

NidaqDataSource::NidaqDataSource()
{
}

// Constructor: initialize and call NiDAQmx setup functions
NidaqDataSource::NidaqDataSource(ArteNeuralDaqOptPb &daq_opt_pb, std::shared_ptr <aTimer> timer)
{

  // Copy option fields
  dev_name = daq_opt_pb.dev_name();
  id = daq_opt_pb.id();
  n_samps_per_buffer = daq_opt_pb.buffer_time_samps();
  n_chans = daq_opt_pb.buffer_n_chans();
  is_master = daq_opt_pb.is_master();
  is_multiplexing = daq_opt_pb.is_multiplexing();
  out_filename = daq_opt_pb.out_filename();


  // Initialize data
  int samp_rate = (int)1e6/32;
  data.voltage_buffer.resize(boost::extents[n_chans][n_samps_per_buffer]);

  // Copy reference to timer
  timer_p = timer;

  // Initialize daq card
  task_handle = 0;
  char taskname[50];
  sprintf( taskname, "AITask on %s", dev_name.c_str() );
  nidaq_err_check( (DAQmxCreateTask(taskname, &task_handle)) );
  
  if( !is_multiplexing ){
    for (int c = 0; c < n_chans; c++){
      char channel_name[50];
      sprintf( channel_name, "%s/ai%d", dev_name.c_str(), c );
      nidaq_err_check ( DAQmxCreateAIVoltageChan(task_handle, channel_name, "",
					       DAQmx_Val_RSE, NEURAL_DAQ_V_MIN,
					       NEURAL_DAQ_V_MAX, DAQmx_Val_Volts,
					       NULL) );
    }
    nidaq_err_check ( DAQmxCfgSampClkTiming(task_handle, "", samp_rate,
					  DAQmx_Val_Rising, DAQmx_Val_ContSamps,
					  n_chans * n_samps_per_buffer) );
  }
  if (is_multiplexing ){
    char channel_name[50];
    sprintf( channel_name, "%s/ai0", dev_name.c_str() );
    nidaq_err_check ( DAQmxCreateAIVoltageChan(task_handle, channel_name, "",
					     DAQmx_Val_RSE, NEURAL_DAQ_V_MIN,
					     NEURAL_DAQ_V_MAX, DAQmx_Val_Volts,
					     NULL) );

    nidaq_err_check ( DAQmxCfgSampClkTiming(task_handle, "", (samp_rate * n_chans),
					  DAQmx_Val_Rising, DAQmx_Val_ContSamps,
					  n_chans * n_samps_per_buffer) ); 
  }
  
  if( is_master ){
    nidaq_err_check ( DAQmxSetRefClkSrc  (task_handle, "OnboardClock") );
    nidaq_err_check ( DAQmxGetRefClkSrc  (task_handle, clk_src, 256) );
    nidaq_err_check ( DAQmxGetRefClkRate (task_handle, &clkRate) );
    nidaq_err_check ( GetTerminalNameWithDevPrefix(task_handle, 
						 "ai/StartTrigger", trig_name) );

    int everyN = is_multiplexing ? (n_chans * n_samps_per_buffer) : n_samps_per_buffer;
    nidaq_err_check ( DAQmxRegisterEveryNSamplesEvent(task_handle, 
						    DAQmx_Val_Acquired_Into_Buffer,
						    everyN, 0, &EveryNCallback, 
						    (void*) &daq_list) );
  }

  if ( !is_master ){
    nidaq_err_check ( DAQmxSetRefClkSrc  (task_handle, clk_src) );
    nidaq_err_check ( DAQmxSetRefClkRate (task_handle, clkRate) );
  }

  // init the oGlom, if out_filename not empty and daq_o_glom not existing.
  // Filename will be chosen by the first daq, that file holds all daqs'
  // data.
  if ( !(daq_opt_pb.out_filename().empty()) & 
       (daq_o_glom.use_count() < 1) ){
    //    const char * fn = const_cast <const char *> (daq_opt_pb.out_filename().c_str());
    const char * fn = daq_opt_pb.out_filename().c_str();
    daq_o_glom = std::shared_ptr <oGlom> (new oGlom (fn , 'b' ));
  }

  // initialize the output data structure
  scratch_data.one_past_end_timestamp = 0;
  scratch_data.voltage_buffer.resize( boost::extents [n_chans][n_samps_per_buffer] );

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


bool NidaqDataSource::get_is_master(){ return is_master; }


int NidaqDataSource::get_master_daq_key(){
  for( DaqList::iterator it = daq_list.begin(); it != daq_list.end(); it++){
    if ( (it->second)->get_is_master() )
      return it->first;
  }
  return -1; // error code meaning no master found
}


NidaqSourcePtr NidaqDataSource::get_master_daq(){
  return daq_list[ NidaqDataSource::get_master_daq_key() ];
}


void NidaqDataSource::start_all_daqs(){

  NidaqSourcePtr master = get_master_daq();
  int master_id = get_master_daq_key();

  DaqList slave_list = daq_list;
  slave_list.erase( master_id );
  
  // Start slave daqs first; they need to wait for clock from master
  DaqList::iterator it;
  for( it = daq_list.begin(); it != daq_list.end(); it++ ){
    NidaqSourcePtr this_daq = it->second;
    nidaq_err_check ( DAQmxStartTask( this_daq->task_handle ) );
  }
  // Then starte master; everyone gets clock edges starting at the same time
  nidaq_err_check ( DAQmxStartTask ( master->task_handle ) );
  // The daqs will now produce data and the master will call 
  // EveryNSampsCallback once per millisecond (every 32 samples per channel)

}

    
int32 NidaqDataSource::EveryNCallback(TaskHandle task_handle,
				      int32 everyNSamplesEventType,
				      uInt32 nSamples, void *callbackData){

  DaqList *this_daq_list = (DaqList *)callbackData;
  NidaqSourcePtr this_daq;

  int32 read;
  int rc, n;
  long unknown_use_var;
  void *status;
  
for( DaqList::iterator it = this_daq_list->begin(); it != this_daq_list->end(); it++){
    this_daq = it->second;
    this_daq->scratch_data.one_past_end_timestamp = timer_p->get_count() - this_daq->buffer_delay;
    // interface is:
    // DAQmxReadBinaryI16(taskhandle, numSampsPerChan, timeout, fillMode,
    //                    read_array[], array_size_samps, samps_actually_ready, reserved)
    // TODO: clean this up, 
    //    rdata_t *the_array = &(this_daq->scratch_data.voltage_buffer[0]);
    NeuralVoltageBuffer *the_voltage_buffer = &(this_daq->scratch_data);
    rdata_t *addy_of_first_element = &(the_voltage_buffer->voltage_buffer[0][0]);
    nidaq_err_check(DAQmxReadBinaryI16(this_daq->task_handle, 32, 10.0, 
				       DAQmx_Val_GroupByScanNumber,
				       addy_of_first_element,
				       32*32, &read, NULL));
    
    this_daq->set_data( *the_voltage_buffer );
  }

}


int32 NidaqDataSource::DoneCallback(TaskHandle task_handle, 
				    int32 status,
				    void *callbackData){
  
}

const ArteNeuralDaqOptPb *  NidaqDataSource::get_daq_settings(){
  return const_cast <ArteNeuralDaqOptPb*> (&daq_opt);
}

void NidaqDataSource::get_all_daqs_settings( ArteSetupOptPb &setup_opt ){
  setup_opt.clear_daqs();
  for(DaqList::iterator it = daq_list.begin(); it != daq_list.end(); it++){
    // first get reference to a new daq_opt
    ArteNeuralDaqOptPb *this_daq_opt = setup_opt.add_daqs();
    // Then copy values to it
    (*this_daq_opt) = *(it->second->get_daq_settings());
  }
}

void NidaqDataSource::nidaq_err_check( int32 error ){
  char errBuff[2048];
  if( DAQmxFailed(error) ){
    std::cerr << "NidaqDataSource: Caught a DAQmx error..." << std::endl;
    fflush(stdout);
    DAQmxGetExtendedErrorInfo(errBuff,2048);
    std::cerr << "NidaqDataSource: saw the daqmx error num: " 
	      << error << " with message: " 
	      << std::endl << errBuff << std::endl;
    fflush(stdout);
  }else{
    //    std::cout << "No daqmx error." << std::endl;
  }
}

// this is copied from ContinuousAI.c, then from arte/src/util.cpp  
// Not fully sure how it works.
int32 NidaqDataSource::GetTerminalNameWithDevPrefix(TaskHandle taskHandle, 
						    const char terminalName[], 
						    char triggerName[])
{
  int32 error = 0;
  char chan[256];
  char *slash;

  nidaq_err_check( DAQmxGetNthTaskChannel(taskHandle, 1, chan, 256) );
  nidaq_err_check( DAQmxGetPhysicalChanName(taskHandle, chan, chan, 256) );
  if( (slash =strchr(chan,'/')) != NULL){
    *slash = '\0';
    *triggerName++ = '/';
    strcat(strcat(strcpy(triggerName,chan),"/"),terminalName);
  } else
    strcpy (triggerName, terminalName);
  return 0;
}
