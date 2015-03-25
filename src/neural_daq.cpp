#define _FILE_OFFSET_BITS  64
#include "trode.h"
#include "lfp_bank.h"
#include "neural_daq.h"
#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <stdint.h>
#include "arteopt.h"
#include <time.h>

extern bool acquiring;
int master_id; // old way
int master_ind;// new way
int32_t buffer_size;
uint32_t timestamp; // global toy timestamp defined in arteopt.h. I'm setting it to 10*buffer count every 
                       // time we get a new buffer.
                       // corresponds to the timer count when the last sample of the buffer was read
                       // (just like the presumed return of a every-n-samples-triggered 'read_counter()'.)

uint32_t   EveryNCallbackSequence;
#define    EVERY_N_TS_BUFFSIZE 10
uint32_t   ts_buffer[EVERY_N_TS_BUFFSIZE];
int        ts_buff_cursor;
timeval    tim;
double      t, t_0;

FILE *tmp_timestamp;

std::map <int, neural_daq> neural_daq_map;

extern Timer arte_timer;
extern neural_daq * neural_daq_array;
extern int n_neural_daqs;
extern int n_filtered_buffers;
extern int n_trodes;
extern int n_lfp_banks;

int n_process_threads_open = 0;

bool daqs_reading = false;
bool daqs_writing = false;

pthread_t my_threads[MAX_THREADS];

void neural_daq_init(boost::property_tree::ptree &setup_pt){

  acquiring = false;
  // arbitrarily pick a card to be master. maybe later user can choose this through conf file?
  master_id = 0; 
  master_ind =0;

  
tmp_timestamp = try_fopen("tmp.ts", "wb");

  n_process_threads_open = 0;
  EveryNCallbackSequence = 0;
  ts_buff_cursor = 0;
  gettimeofday(&tim,NULL);
  t_0 = tim.tv_sec + (tim.tv_usec / 1000000.0);

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

  int tmp_daq_count = 0;
  BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		setup_pt.get_child("options.setup.neural_daq_list")){
    tmp_daq_count++;
  }
  
  neural_daq_array = new neural_daq [tmp_daq_count];

  // import the settings
  BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		setup_pt.get_child("options.setup.neural_daq_list")){
    // bring in properties from the config file
    ndaq_pt= v.second;
    std::string tmp_dev_name, tmp_in_name, tmp_out_name;
    assign_property <int> ("id", &(this_nd.id), ndaq_pt, ndaq_pt, 1);
    assign_property <uint16_t> ("n_samps_per_buffer", &(this_nd.n_samps_per_buffer), ndaq_pt, ndaq_pt, 1);
    assign_property <uint16_t> ("n_chans", &(this_nd.n_chans), ndaq_pt, ndaq_pt, 1);
    assign_property <std::string> ("dev_name", &tmp_dev_name, ndaq_pt, ndaq_pt, 1);
    assign_property <std::string> ("in_filename", &tmp_in_name, ndaq_pt, ndaq_pt, 1);
    assign_property <std::string> ("raw_dump_filename", &tmp_out_name, ndaq_pt, ndaq_pt, 1);
    
    strcpy( this_nd.dev_name, tmp_dev_name.c_str() );
    strcpy( this_nd.in_filename, tmp_in_name.c_str() );
    strcpy( this_nd.raw_dump_filename, tmp_out_name.c_str() );

    this_nd.total_samp_count = this_nd.n_chans * this_nd.n_samps_per_buffer;
    this_nd.data_ptr = this_nd.data_buffer;
    init_array <rdata_t>(this_nd.data_ptr, 4, (this_nd.n_chans * this_nd.n_samps_per_buffer) );
    this_nd.size_bytes = this_nd.total_samp_count * sizeof(this_nd.data_ptr[0]);
    this_nd.daq_buffer_count = 0;
    this_nd.this_buffer = 0;
    this_nd.buffer_time_interval = 0.001;
    neural_daq_map.insert( std::pair<int,neural_daq> (this_nd.id, this_nd) );
    neural_daq_array[n_neural_daqs] = this_nd;
    n_neural_daqs++;
  }

  init_files(); // open files for reading, files for writing, where appropriate

  //int n_daq = neural_daq_map.size(); // OLD WAY
  int n_daq = n_neural_daqs; // NEW WAY
  int n = master_id; // old way
  n = master_ind; // new way
  
  if( !daqs_reading ){   // that is, if we're really using the cards, as opposed to taking data from raw_dump files

    while(n_completed < n_daq){
      if(!master_completed){   // we want to init the master card first
	n = master_id;         // at end of that init, set n to 0,
      } else{                  // then iterate through the other cards
	if (n == master_id){
	  n++;}
      }
      //this_nd = neural_daq_map[n];  // old way
      this_nd = neural_daq_array[n];// new way
      //it = neural_daq_map.find(n);
      //this_nd = (*it).second;
      
      this_nd.task_handle = 0; // dunno why, but most examples do this 0 init
      char taskname [50];
      sprintf(taskname, "AITask on %s", this_nd.dev_name);
      buffer_size = buffer_samps_per_chan * this_nd.n_chans;
      daq_err_check_end( (DAQmxCreateTask(taskname, &(this_nd.task_handle))), this_nd );

      
      for (int c = 0; c < this_nd.n_chans; c++){
	sprintf(channel_name, "%s/ai%d", this_nd.dev_name, c);
	daq_err_check_end ( DAQmxCreateAIVoltageChan(this_nd.task_handle,channel_name,"",DAQmx_Val_RSE,NEURAL_DAQ_V_MIN,NEURAL_DAQ_V_MAX,DAQmx_Val_Volts,NULL), this_nd );
      }  
      daq_err_check_end ( DAQmxCfgSampClkTiming(this_nd.task_handle, "", samp_rate, DAQmx_Val_Rising, DAQmx_Val_ContSamps, buffer_size), this_nd );
  
      if( n == master_id ){
	std::cout << "Processing master daq" << std::endl;
	master_daq = this_nd;
	daq_err_check_end ( DAQmxSetRefClkSrc( this_nd.task_handle, "OnboardClock"), this_nd );
	if(n_daq > 1)
	  daq_err_check_end ( DAQmxGetRefClkSrc( master_daq.task_handle, clk_src, 256), this_nd );
	daq_err_check_end ( DAQmxGetRefClkRate( master_daq.task_handle, &clkRate), this_nd );
	if(n_daq > 1)
	  daq_err_check_end ( GetTerminalNameWithDevPrefix(master_daq.task_handle, "ai/StartTrigger", trig_name), this_nd );
	daq_err_check_end ( DAQmxRegisterEveryNSamplesEvent( master_daq.task_handle, DAQmx_Val_Acquired_Into_Buffer, 32,0,EveryNCallback,(void *)&neural_daq_array), this_nd );
	daq_err_check_end ( DAQmxRegisterDoneEvent(master_daq.task_handle, 0, DoneCallback, (void *)&master_daq), this_nd );
	std::cout << "Done processing master daq." << std::endl;
	
///// 	daq_err_check( DAQmxCreateTask(	"master_counter_generation_task",
///// 					&(this_nd.counter_generation_task) ));
///// 	daq_err_check( DAQmxCreateTask("master_counter_count_task",
///// 				       &(this_nd.counter_count_task) ));

	////// This is done in timer.cpp instead of here
///// 	char co_chan_name[40];
///// 	char ci_chan_name[40];
///// 	char ci_trig_chan_name[40];
///// 	sprintf( co_chan_name, "%s/ctr0", this_nd.dev_name);
///// 	sprintf( ci_chan_name, "%s/ctr1", this_nd.dev_name);
///// 	sprintf( ci_trig_chan_name, "/%s/PFI9", this_nd.dev_name);

///// 	daq_err_check( DAQmxCreateCOPulseChanTicks( this_nd.counter_generation_task,
///// 						   co_chan_name, "", "OnboardClock",
///// 						   DAQmx_Val_Low, 0, 40000, 40000) );
///// 	daq_err_check( DAQmxCfgImplicitTiming( this_nd.counter_generation_task,
///// 					       DAQmx_Val_ContSamps, 1000) );

///// 	daq_err_check( DAQmxCreateCICountEdgesChan( this_nd.counter_count_task,
///// 						    ci_chan_name, "", 
///// 						    DAQmx_Val_Rising, 0, DAQmx_Val_CountUp) );
///// 	daq_err_check( DAQmxCfgSampClkTiming( this_nd.counter_count_task,
///// 					      ci_trig_chan_name, 1000.0, DAQmx_Val_Rising,
///// 					      DAQmx_Val_ContSamps, 1000) );

///// 	daq_err_check( DAQmxSetRefClkSrc( this_nd.counter_generation_task, "OnboardClock") );
///// 	daq_err_check( DAQmxSetRefClkSrc( this_nd.counter_count_task, "OnboardClock") );
						    

	master_completed = true;
	n = 0;
      } else {
	std::cout << "Processing a slave daq." << std::endl;
	daq_err_check_end ( DAQmxSetRefClkSrc( this_nd.task_handle, clk_src), this_nd );
	daq_err_check_end ( DAQmxSetRefClkRate(this_nd.task_handle, clkRate), this_nd );
	daq_err_check_end ( DAQmxCfgDigEdgeStartTrig( this_nd.task_handle, trig_name, DAQmx_Val_Rising), this_nd );
	// Register every n samples?  No.  we only want the master card to do this.
	daq_err_check_end ( DAQmxRegisterDoneEvent(this_nd.task_handle, 0, DoneCallback, (void *)&this_nd), this_nd );
      }
      this_nd.status = 0;
      //neural_daq_map[this_nd.id] = this_nd; // we gained a task handle for each nd. must re-insert into the map for that value to persist
      neural_daq_array[n] = this_nd; // I think this is redundant.  modifications to this_nd have been happening in the array already, right?
      n_completed++;
      std::cout << "Done processing some daq." << std::endl;
    }
    std::cout << "Done with daq_processing loop. " << std::endl;
  } //end if for card-read specefic stuff 
  
}

void neural_daq_start_all(void){
  arte_timer.toy_timestamp = 0;
  //acquiring = true;
  if (! daqs_reading){
    // start all slave tasks first, so that they don't miss the start trigger from the master
    std::map<int, neural_daq>::iterator it;
    //daq_buffer_count = 0;
    for(int n = 0; n < n_neural_daqs; n++){
      neural_daq *nd = &( neural_daq_array[n] );

      

      if(nd->id != master_id){
	printf("about to start a slave daq task\n");
	daq_err_check_end ( DAQmxStartTask( nd->task_handle ), this_nd );
	printf("just finished a slave daq task\n");
	nd->status= 1;
      }
    }
    // start the master task last ->      
    
    ////    printf("about to start master daq count generation and count count tasks.\n");
    ////daq_err_check ( DAQmxStartTask( neural_daq_array[master_id].counter_generation_task ) );
    ////daq_err_check ( DAQmxStartTask( neural_daq_array[master_id].counter_count_task ) );
    
    printf("about to start the master daq AI task.\n");
    daq_err_check_end ( DAQmxStartTask( neural_daq_array[master_id].task_handle), this_nd );
    printf("finished starting the master daq task.\n");
    neural_daq_array[master_id].status = 1;
  } else {  // then daqs are getting their data from files
    
    printf("Reading from file %s\n", neural_daq_map[0].in_filename);
    neural_daq *nd = &(neural_daq_array[0]);
    uint32_t file_n_buffers;
    uint16_t file_n_chans, file_buff_len;
    try_fread<uint32_t>( &file_n_buffers, 1, nd->in_file );
    try_fread<uint16_t>( &file_n_chans, 1, nd->in_file );
    try_fread<uint16_t>( &file_buff_len, 1, nd->in_file );
    //TODO: Sanity checks - does file n_chans & n_samps_per_buffer match those of this neural daq?
    std::cout << "file_n_buffers: " << file_n_buffers << "  file_n_chans: " << file_n_chans << "  file_buff_len: " << file_buff_len << std::endl;
    while (neural_daq_array[master_id].this_buffer < file_n_buffers){
      arte_timer.toy_timestamp += 10; // 10 raw time units is 1 ms is 1 buffer
      read_data_from_file();
      //nanosleep( nd->buffer_time_interval * 1000000000.0);  // <-- commented b/c I still have to read the docs
    }
  } // end if for reading from file

}

void neural_daq_stop(int i){
  neural_daq *nd = &(neural_daq_array[i]);
  bool32 task_done;
  if( nd->task_handle > 0 ) {
    printf("Ok after if th is:%d\n", nd->task_handle); fflush(stdout);
    daq_err_check_end( DAQmxIsTaskDone( nd->task_handle, &task_done ), nd_task );
    printf("Ok ofter IsTaskDone check\n"); fflush(stdout);
    if( !task_done ){
      printf("about to stop AI task\n"); fflush(stdout);
      daq_err_check_end( DAQmxStopTask ( nd->task_handle), nd_task );
      printf(" finished stopping AI task, about co clear it\n");fflush(stdout);
    } else {
      printf("neural daq %d told to stop its AI task, but task is already done.\n", nd->id ); fflush(stdout);
    }
    printf("Ok before cleartask\n"); fflush(stdout);
    daq_err_check_end( DAQmxClearTask( nd->task_handle), nd_task );
    if (i == master_ind){
      arte_timer.stop2();
    }
  }
}

void neural_daq_stop_all(void){
  if(acquiring){
    acquiring = false;
    printf(" NEURAL DAQ STOP ALL \n"); fflush(stdout);
    sleep(1);  // why sleep?  b/c for some reason hitting immediately after running program hangs the computer (threads' fault?)
    neural_daq *nd;
    
    bool32 isDone;
    if( !daqs_reading ){
      for(int n = 0; n < n_neural_daqs; n++){
	printf("About to neural_daq_stop(%d)\n",n); fflush(stdout);
	neural_daq_stop(n);  
	printf("Finished neural_daq_stop(%d)\n",n); fflush(stdout);
      }
    }
    bool master_done = false;  // <-- WHY?
    
    finalize_files();
    
    printf("got ast finalize_files()\n"); fflush(stdout);
    
    for(int n = 0; n < n_filtered_buffers; n++){
      //std::cout << "About to call fb[n].finalize_files() from neural_daq.cpp" << std::endl;
      filtered_buffer_array[n].finalize_files();
    }
    
    for(int n = 0; n < n_trodes; n++){
      trode_array[n].end_acquisition();
    }
    
    for(int n = 0; n < n_lfp_banks; n++){
      lfp_bank_array[n].end_acquisition();
    }
    
    printf(" DONE WITH NEURAL DAQ STOP ALL \n"); fflush(stdout);
    fclose(tmp_timestamp);
  } else {
    printf("neural_daq_stop_all issued, but acquiring = false.\n"); fflush(stdout);
  }
}

void read_data_from_file(void){ // the file-reading version of EveryNCallback

  neural_daq *nd;
  for (int n = 0; n < n_neural_daqs; n++){
    nd = & (neural_daq_array[n]);
    buffer_size = nd->n_chans * nd->n_samps_per_buffer;
    try_fread<rdata_t>( nd->data_ptr, buffer_size, nd->in_file );
    if( nd->out_file != NULL ){
      printf("writing buffer size: %d\n", buffer_size);
      try_fwrite<rdata_t>( nd->data_ptr, buffer_size, nd->out_file );
    }
    nd->this_buffer += 1;
    nd->daq_buffer_count += 1;
    nd->buffer_timestamp = arte_timer.getTimestamp();
  }

  for(int i = 0; i < n_trodes; i++){
    trode_filter_data( (void*) &(trode_array[i]));
  }
  for(int i = 0; i < n_lfp_banks; i++){
    lfp_bank_filter_data( (void*) &(lfp_bank_array[i]) );
    
  }

}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNSamplesEventType, uInt32 nSamples, void *callbackData){

 
  neural_daq *nd;
  int32 read;
  int rc;
  int n;
  long unknown_use_var;
  pthread_t process_cycle_thread;
  pthread_t pc[EVERY_N_TS_BUFFSIZE];
  int in_use_pool[EVERY_N_TS_BUFFSIZE];
  int available_pool[EVERY_N_TS_BUFFSIZE];
  int n_in_use;
  int n_availabe;
  bool thread_flagged_for_clear[EVERY_N_TS_BUFFSIZE];
  pthread_attr_t attr;
  uint32_t simple_ts;
  
  if(acquiring){
    gettimeofday(&tim,NULL);
    t = tim.tv_sec + (tim.tv_usec / 1000000.0);
    uint32_t sys_time = (uint32_t) ( (t-t_0) * 10000.0 );
    //printf("t_0:%.6lf  t:%.6lf  t-t_0:%.6lf  sys_time:%d\n", t_0, t, t-t_0, sys_time);
    //printf("%d\n",sys_time);
    
    
    //try_fwrite<uint32_t>( &(ts_buffer[ts_buff_cursor]), 1, tmp_timestamp);
    try_fwrite<uint32_t>( &sys_time, 1, tmp_timestamp);
    //ts_buffer[ts_buff_cursor] = 100000;
  }
  
  //printf("in everynsamples\n"); fflush(stdout);
  
  if(false){
    
    ts_buffer[ts_buff_cursor] = arte_timer.getTimestamp();
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    rc = pthread_create(&(pc[ts_buff_cursor]), 
			&attr, 
			process_cycle, 
			(void *) &(ts_buffer[ts_buff_cursor]));
    
    n_process_threads_open++;
    if(rc){
      printf("ERROR: return code from pthread_create() in EveryNCallback in neural_daq.cpp is %d\n", rc);
      neural_daq_stop_all();
      exit(1);
    }
    if(acquiring & (n_process_threads_open > 1)){
      printf("ERROR: %d PROCESS_CYCLE THREADS OPEN - WE RAN OUT OF PROCESSING TIME.\n",n_process_threads_open);
    }
    pthread_attr_destroy(&attr);
  } 
  
  if(true){
    simple_ts = arte_timer.getTimestamp();
    ts_buffer[ts_buff_cursor] = simple_ts;
    //printf("about to do_cycle\n"); fflush(stdout);
    do_cycle(simple_ts);
    //printf("finished do_cycle\n"); fflush(stdout);
  }
  //printf("ts_buff_cursor:%d\n",ts_buff_cursor); fflush(stdout);
  //printf("Still in everynsamples\n"); fflush(stdout);
  
  // process_cycle( (void*)(ts_buffer+ts_buff_cursor) );  
  ts_buff_cursor++;
  if(ts_buff_cursor == EVERY_N_TS_BUFFSIZE){
    ts_buff_cursor = 0;
  }

  //printf("GOT to the end of everyNCallback\n"); fflush(stdout);

}

void *process_cycle(void *thread_data){
  //printf("in process_cycle\n"); fflush(stdout);
  uint32_t this_cycle_time = * (uint32_t*)thread_data;
  do_cycle(this_cycle_time);
}

void do_cycle(timestamp_t this_cycle_time){
 
  //uint32_t this_cycle_time = *(uint32_t*)thread_data;
  neural_daq *nd;
  int32 read;
  int rc;
  //pthread_attr_t attr;
  //pthread_t daq_thread[MAX_NEURAL_DAQS];
  //pthread_t trode_thread[MAX_TRODES];
  //pthread_t lfp_bank_thread[MAX_LFP_BANKS];
  int n;
  long this_p;
  void *status;
  //printf("In process_cycle\n");
  //pthread_attr_init(&attr);
  //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  //printf("An Ok one\n");
 // We may want to use a nonmember function here rather than a method.  Overhead issue when we call it at 1kHz
  // uint32_t time_now = arte_timer.getTimestamp();
  
  // for(n = 0; n < neural_daq_map.size(); n++){
  for( n = 0; n < n_neural_daqs; n++){
    //std::cout << "In data-probing loop for neural_daq: " << n << std::endl;
    //fflush(stdout);
    
    nd = &(neural_daq_array[n]);
    nd->buffer_timestamp = this_cycle_time - (nd->n_samps_per_buffer-1)*10/32;
    //printf("Buffer timestamp:%d\n",nd->buffer_timestamp);
    //nd->buffer_timestamp = 100000L;
    //daq_err_check ( DAQmxReadAnalogF64( nd->task_handle, 32, 10.0, DAQmx_Val_GroupByScanNumber, nd->data_ptr, buffer_size, &read,NULL) );
    
    //     rc = pthread_create(&(daq_thread[n]), &attr, process_daq, (void *)nd);
    
    //     if(rc){
    //       printf("ERROR MAKING DAQ_THREAD.  Code: %d\n",rc);
    //     }
    
    if(acquiring){
      //   uint32_t time_now;
      //time_now = arte_timer.getTimestamp();
      if( nd->out_file != NULL){
 	try_fwrite<rdata_t>( nd->data_ptr, buffer_size, nd->out_file);
      }
      //nd->buffer_timestamp = time_now - 
      //	(nd->n_samps_per_buffer-1) * 10/32; // make time correspond to the buffer's first data point
      nd->daq_buffer_count += 1;
      //nd->buffer_timestamp = nd->daq_buffer_count * 10;
    }
    
    //printf("About to read AD\n");fflush(stdout);
    bool32 tmp_isdone;
    daq_err_check_end( DAQmxIsTaskDone( nd->task_handle, &tmp_isdone ), this_nd);
    if(!tmp_isdone)
      daq_err_check_end ( DAQmxReadBinaryI16( nd->task_handle, 32, 10.0, DAQmx_Val_GroupByScanNumber, nd->data_ptr, buffer_size, &read,NULL), this_nd );
    //printf("Done read\n"); fflush(stdout);
  }
  //pthread_attr_destroy(&attr);
  
  //   for (n = 0; n < n_neural_daqs; n++){
  //     pthread_join(daq_thread[n], &status);
  //   }
  
  //printf("about to enter if(acquiring) block (acquiring=%d)\n",acquiring);fflush(stdout);
  if(acquiring){
    //pthread_attr_init(&attr);
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // many threads, each is a leaf, so, detached
    //time_now
    arte_timer.toy_timestamp += 10;
    //std::cout << "Finished daq loop." << std::endl;
    //fflush(stdout);
    n = 0; // for threads
    //for(std::map<uint16_t, Trode>::iterator it = trode_map.begin(); it != trode_map.end(); it++){
    //  Trode *this_trode = &((*it).second);
    for(n = 0; n < n_trodes; n++){
	
      Trode * trode_pt = &(trode_array[n]);
    
//       rc = pthread_create(&(trode_thread[n]), &attr, process_trode, (void *)trode_pt);      
//       if(rc){
// 	printf("ERROR CREATING TRODE THREAD Code: %d\n", rc);
//       }
      //printf("about to trode_filter_data\n"); fflush(stdout);
      trode_filter_data((void*)trode_pt);
      //printf("done trode_filter_data\n"); fflush(stdout);
      

    }
    //for( std::map< uint16_t, Lfp_bank >::iterator it = lfp_bank_map.begin();
    //	 it!= lfp_bank_map.end(); it++) {
    //Lfp_bank *this_bank = &((*it).second);
    //std::cout << "About to loop lfp_banks." << std::endl;
    //fflush(stdout);
    for(n = 0; n < n_lfp_banks; n++){
      Lfp_bank * this_bank = &(lfp_bank_array[n]);
      
      //      rc = pthread_create(&(lfp_bank_thread[n]), &attr, process_lfp_bank, (void *)this_bank);
      //if(rc){
      //printf("ERROR CREATING LFP_BANK THREAD Code: %d\n",rc);
      //}

      //std::cout << "array assignment is Ok." << std::endl;
      //std::cout << "n_lfp_banks is: " << n_lfp_banks << std::endl;
      //std::cout << "n_trodes is : " << n_trodes << std::endl;
      //fflush(stdout);
      //printf("about to lfp_bank_filter_data\n"); fflush(stdout);
      lfp_bank_filter_data( (void*)&lfp_bank_array[n] );
      //  printf("finished lfp_bank_filter_data\n"); fflush(stdout);

      //std::cout << "finished one lfp bank." << std::endl;
      //fflush(stdout);
    }
    //pthread_attr_destroy(&attr);
  }
  //printf("finished if(acquiring) block\n"); fflush(stdout);
  //printf("closing the thread\n");
  //n_process_threads_open--;
  //pthread_exit(NULL);
}

void *process_daq(void *thread_data){

  neural_daq *nd = (neural_daq*)thread_data;
  int32 read;

  if(acquiring){
    uint32_t time_now;
    //time_now = arte_timer.getTimestamp();
    if( nd->out_file != NULL){
      try_fwrite<rdata_t>( nd->data_ptr, buffer_size, nd->out_file);
    }
    nd->buffer_timestamp = time_now - 
      (nd->n_samps_per_buffer-1) * 10/32; // make time correspond to the buffer's first data point
    nd->daq_buffer_count += 1; 
  }
  
  daq_err_check_end ( DAQmxReadBinaryI16( nd->task_handle, 32, 10.0, DAQmx_Val_GroupByScanNumber, nd->data_ptr, buffer_size, &read,NULL), this_nd );
  
  pthread_exit(NULL);
}  

void *process_trode(void *thread_data){
  Trode *trode_pt = (Trode *)thread_data;
  trode_filter_data( (void*) trode_pt );
  pthread_exit(NULL);
}
  
void *process_lfp_bank(void *thread_data){
  Lfp_bank *this_bank = (Lfp_bank *)thread_data;
  lfp_bank_filter_data( (void*)this_bank );
  pthread_exit(NULL);
}

int32 CVICALLBACK DoneCallback( TaskHandle taskHandle, int32 status, void *callbackData){
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
  //std::map<int, neural_daq>::iterator it;
  for(int i = 0; i < n_neural_daqs; i++){
    if( neural_daq_array[i].task_handle = taskhandle){
      return neural_daq_array[i];
    }
  }
  std::cout << "Couldn't find the taskhandle: " << taskhandle << std::endl;
}

void print_buffer(neural_daq *ndp, int row_lim, int col_lim, int row_length){
  system("clear");
  std::cout << std::fixed << std::setprecision(1);
  for (int r = 0; r < row_lim; r++){
    for (int c = 0; c < row_lim; c++) {
      std::cout << std::setw(5) << ndp->data_ptr[ r*row_length + c ] << " ";
    }
    std::cout << std::endl;
  }
}

void init_files(void){
  for(int n = 0; n < n_neural_daqs; n++){
    neural_daq *nd = &(neural_daq_array[n]);
  printf("about to try to open file from neuraldaq622: %s\n", (nd->in_filename));
  printf("about to try to open file from neuraldaq628: %s\n", (nd->raw_dump_filename));

    if (strcmp(nd->in_filename,"")==0){
	printf ("error: setting up neural_daqs: in_filename is empty on n=%i out of %i\n", (n),(n_neural_daqs));
	}

    if (strcmp(nd->raw_dump_filename,"")==0){
	printf ("error: setting up neural_daqs: raw_dump_filename is empty on n=%i out of %i\n", (n),(n_neural_daqs));
	exit(1);
	}


    if (! (strcmp(nd->in_filename,"none")==0) ){
      nd->in_file = try_fopen( nd->in_filename, "rb" );
      daqs_reading = true;
    }


    if(! (strcmp(nd->raw_dump_filename,"none")==0 )){
      nd->out_file = try_fopen( nd->raw_dump_filename, "wb" );
      try_fwrite( &(nd->daq_buffer_count), 1, nd->out_file );
      try_fwrite( &(nd->n_chans),          1, nd->out_file );
      try_fwrite( &(nd->n_samps_per_buffer), 1, nd->out_file );
    }

  }
}

void finalize_files(void){
  for(int n = 0; n < n_neural_daqs; n++){
    neural_daq *nd = &(neural_daq_array[n]);

    if(! (nd->out_file == NULL)){
      rewind(nd->out_file);
      try_fwrite( &(nd->daq_buffer_count), 1, nd->out_file );
      fclose( nd->out_file );
    }

    if(! (nd->in_file == NULL)){
      fclose( nd->in_file);
    }

  }
}
