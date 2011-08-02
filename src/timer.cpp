#include "timer.h"
#include "util.h"
#include <iostream>
#include <time.h>

int const N_MAST_SYNC_TX = 10;
int const N_SLV_SYNC_TX = 1;
Timer::Timer(){
	std::cout<<"Timer::Timer start"<<std::endl;
	timerID = -1;
	running = false;
	armed = false;

	isSynced = false;
	
	initCount = 0;
	countEdge= DAQmx_Val_Rising;
	countDir = DAQmx_Val_CountUp;

	syncCount=0;

	becomeMaster(false);

	//         niDev       niCtr          niArm           niSync
	//	setDevStrs("/Dev1/", "/Dev1/ctr0", "/Dev1/PFI2", "/Dev1/port0/line0:7");

	counterTask = 0;
	counter_count_task = 0;
	counter_generation_task = 0;
	diPulseTask = 0;

	toy_timestamp = 0;

	std::cout<<"Timer::Timer end"<<std::endl;

	next_ok_ts_to_print = 0;
}

Timer::~Timer(){
	stop();
}

bool Timer::isMaster(){
	return master;
}
int Timer::becomeMaster(bool isMaster){
	std::cout<<"Timer::becomeMaster using:"<<isMaster<<std::endl;
	master = isMaster;

	return 0;
}

int Timer::setDevStrs(std::string const& dev, std::string const& ctr, 
std::string const& arm, std::string const& sync){

	// the NIDAQmx functions require char arrays but for convinience we'll let the user
	// specify the devices as strings
	strcpy(niDev, dev.c_str());
	strcpy(niCtr, ctr.c_str());
	strcpy(niArm, arm.c_str());
	strcpy(niSync, sync.c_str());

	return 0;
}

int Timer::start(){
	std::cout<<"Timer::start() called!"<<std::endl;

	if(!ad_slave){
	if (running){
		std::cout<<"ERROR!!!! The Timer is already running\n";
		return -1;
	}
	if (counterTask!=0){
		std::cout<<"ERROR!!!! This timer already has a counter task\n";
		return -2;
	}

	initDAQSyncTask();

	if (!master)
		rxSyncCount();

	initDAQCounterTask();
	armCounterTask();

	std::cout<<"Timer::start(), starting the NIDAQmx Tasks"<<std::endl;
	daq_err_check(DAQmxStartTask(counterTask));
	daq_err_check(DAQmxStartTask(diPulseTask));
	}

	if(ad_slave){
	  printf("about to start counter task \n");
	  //daq_err_check( DAQmxStartTask( counterTask ) );
	  printf("finished starting counter task.\n");
	}
	return 0;
}
int Timer::stop(){
	std::cout<<"Timer::stop() start"<<std::endl;

	running = false;
	if (counterTask!=0){
		std::cout<<"Timer::stop() tasks are non zero, stopping them"<<std::endl;
		DAQmxStopTask(counterTask);
		DAQmxStopTask(diPulseTask);
		DAQmxClearTask(counterTask);
		DAQmxClearTask(diPulseTask);
		counterTask = 0;
		diPulseTask = 0;
	}

	if(ad_slave){
	  printf("AD SLAVE!\n");
	  daq_err_check( DAQmxStopTask( counter_generation_task ) );
	  daq_err_check( DAQmxStopTask( counter_count_task ) );
	  daq_err_check( DAQmxClearTask( counter_generation_task ) );
	  daq_err_check( DAQmxClearTask( counter_count_task ) );
	}

	std::cout<<"Timer::stop() end"<<std::endl;
	return 0;
}

int  Timer::setInitCount(uint32_t newCount){
	std::cout<<"Timer::setInitCount() called with:"<<newCount<<std::endl;
	if (running){
		std::cout<<"ERROR!!!!!!!, the timer is already running,  can't change the init count\n";
		return -1;
	}
	else if (!master)
		std::cout<<"WARNING: If not master then setting the initial count value has no meaning!\n";

	initCount = newCount;
	return 0;
}

uint32_t Timer::getCount(){
	if (counterTask==0){
		return 0;
	} 
	uInt32 count = 0;
	daq_err_check(DAQmxReadCounterScalarU32(counterTask, 10.0, &count, NULL));
	std::cout<<"Timer::getCount() called, count is:"<<count<<std::endl;
	//get the count from the counter card and return it
	return (uint32_t) count;
	
}

#ifndef ISTOY
timestamp_t Timer::getTimestamp(){
  std::cout<<"Timer::getTimestamp() called!"<<std::endl;
    uInt32 timestamp;
    ////daq_err_check( DAQmxReadCounterScalarU32(counterTask, 10.0, &timestamp, NULL) );
    daq_err_check ( DAQmxReadCounterScalarU32( counter_count_task, 10.0 &timestamp, NULL ) );
  //get the count from the counter card, convert to TS and return it
    return (timestamp_t)timestamp;
}
#endif

#ifdef ISTOY
uint32_t Timer::getTimestamp(){
  uInt32 timestamp = 0;
  ////daq_err_check( DAQmxReadCounterScalarU32( counterTask, 10.0, &timestamp, NULL) );
  if( counter_count_task ){
    //printf("sanity check");
    daq_err_check( DAQmxReadCounterScalarU32( counter_count_task, 10.0, &timestamp, NULL) );
  } else {
    printf("No running counter_count task yet.\n");
    timestamp = 0;
  }
  if(timestamp > next_ok_ts_to_print){
    next_ok_ts_to_print += 2500;
    printf("TS is %d\n",timestamp);
  }
  return timestamp;
  //return toy_timestamp;
}
#endif

int Timer::initDAQCounterTask(){
	std::cout<<"Timer::initDAQCounterTask() called!"<<std::endl;
	//check to see if the task already exists if so end
	if (counterTask!=0)
		std::cout<<"ERROR!!!! The counterTask is already running, cant setup a new one\n";
	daq_err_check(DAQmxCreateTask("CounterTask", &counterTask));
	std::cout<<"\tsetting up counter to trigger off of line:"<<niCtr<<std::endl;
	daq_err_check(DAQmxCreateCICountEdgesChan(counterTask, niCtr, "", countEdge, initCount, countDir));

	return 0;
}

int Timer::initDAQSyncTask(){
	std::cout<<"Timer:initDAQSyncTask() called!"<<std::endl;
	DAQmxCreateTask("DigitalDetectionTask", &diPulseTask);
	DAQmxCreateDIChan(diPulseTask, niSync, "", DAQmx_Val_ChanPerLine);
	DAQmxCfgChangeDetectionTiming(diPulseTask, NULL, niSync, DAQmx_Val_ContSamps,1);
	DAQmxRegisterSignalEvent(diPulseTask,DAQmx_Val_ChangeDetectionEvent,0,timerDigTrigCallback, this);

	return 0;
}

int Timer::armCounterTask(){

  if(!ad_slave){
    std::cout<<"Timer::armCounterTask() called!"<<std::endl;
    std::cout<<"\tsetting the counter to trigger off of:"<<niArm<<std::endl;
    daq_err_check(DAQmxSetArmStartTrigType(counterTask, DAQmx_Val_DigEdge));
    daq_err_check(DAQmxSetDigEdgeArmStartTrigSrc(counterTask, niArm));
    daq_err_check(DAQmxSetDigEdgeArmStartTrigEdge(counterTask, DAQmx_Val_Rising));
  }

  // in the ad slave case, we don't want a start trigger.  Just start counting
  // the clock pulses as soon as the task is started (started by software, during
  // the pause in clock pulses caused by ad master clock reset)
  if(ad_slave){
    running = 0;  // in my case.  In non-ad-slave, too?
    daq_err_check( DAQmxDisableStartTrig(counterTask) );
  }

  return 0;

}

int Timer::txSyncCount(uint32_t syncCount, int nPackets)
{
	return 0;
}
uint32_t Timer::rxSyncCount()
{
	std::cout<<"Warning: Not RXing packets!!!!!!!!!!!\n";
	initCount =  12345;
	// listen on the time port for the packet
	// do while the packet TS is !less than 1 second off from now
	// if more than 10 packets are received that have a bad timestamp throw a clock
	// clock sync error
	// use current packet
	return 0;
}

int Timer::setTimerID(int newId){
	timerID = newId;
	return 0;
}
int Timer::getTimerID(){
	return timerID;
}

uint32_t Timer::getSyncCount(){
	return syncCount;
}
int Timer::setSyncCount(uint32_t count){
	syncCount = count;
	return 0;
}

void Timer::initUdpTx(std::string host, int port){
  //TEMPORARY commented by Greg b/c of error
  //txDat = NetCom::initUdpTx(host.c_str(), port);	
}
void Timer::initUdpRx(std::string ip, int port){
  //TEMPORARY commented by Greg b/c of error
  //rxDat = NetCom::initUdpRx(host.c_str(), port);
}

int32 CVICALLBACK timerDigTrigCallback(TaskHandle taskHandle, int32 signalID, void *callbackData){
//	std::cout<<"TimerDigTrigCallback Fired"<<std::endl;
	
	Timer *t = (Timer *) callbackData;

	uint32_t cSyncCnt=t->getCount();
		
	if (t->isMaster()){

		uint32_t pSyncCnt=t->getSyncCount();
		t->setSyncCount(cSyncCnt);
		if (pSyncCnt==0)
			return 0;

		uint32_t nextSyncCount = cSyncCnt + (cSyncCnt-pSyncCnt);
		std::cout<<"\n\t predicted next sync count:"<<nextSyncCount<<std::endl;
		t->txSyncCount(nextSyncCount, N_MAST_SYNC_TX);
	}
	else
		t->txSyncCount(cSyncCnt, N_SLV_SYNC_TX);
	
	return 0;
}

void Timer::init2(boost::property_tree::ptree &timer_pt){
  std::string clock_source, dev_name_tmp, ctr_name_tmp, timer_role_tmp, niArm_tmp, niSync_tmp;
  
  assign_property<std::string> ("clock_source", &clock_source,   timer_pt, timer_pt, 1);
  assign_property<std::string> ("dev_name",     &dev_name_tmp,   timer_pt, timer_pt, 1);
  assign_property<std::string> ("ctr_name",     &ctr_name_tmp,   timer_pt, timer_pt, 1);
  assign_property<std::string> ("timer_role",   &timer_role_tmp, timer_pt, timer_pt, 1);
  assign_property<uint16_t>    ("timer_freq",   &timer_freq,     timer_pt, timer_pt, 1);
  assign_property<std::string> ("niArm",        &niArm_tmp,      timer_pt, timer_pt, 1);
  assign_property<std::string> ("niSync",       &niSync_tmp,     timer_pt, timer_pt, 1);

  if( strcmp(clock_source.c_str(), "ad") == 0 ){
    ad_slave = true;
    becomeMaster(false);
  }

  strcpy( niDev, dev_name_tmp.c_str() );
  strcpy( niCtr, ctr_name_tmp.c_str() );
  strcpy( niArm, niArm_tmp.c_str() );
  strcpy( niSync, niSync_tmp.c_str() );

  if(ad_slave){
    //setInitCount(0);
    // initDAQSyncTask();  // Not in ad-slave mode
    //initDAQCounterTask();
    //armCounterTask();
    counterTask = 0;
    ///// printf("About to create counter task.\n");
/////     fflush(stdout);
/////     daq_err_check( DAQmxCreateTask("Counter Task",&counterTask) );
/////     fflush(stdout);
/////     printf("About to create edge counter chan.\n");
/////     fflush(stdout);
/////     daq_err_check( DAQmxCreateCICountEdgesChan(counterTask, "Dev2/ctr1","",DAQmx_Val_Rising, 0, DAQmx_Val_CountUp) );
/////     fflush(stdout);

/////     //    printf("about to try to set sample clock timing for counter channel\n");
/////     //daq_err_check( DAQmxCfgSampClkTiming( counterTask, "/Dev1/10MHzRefClock", 10000000.0, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 100) );
/////     //printf("finished setting sample clock timing for counter channel\n");
    
/////     printf("about to try setRefSource/n");
/////     daq_err_check( DAQmxSetRefClkSrc( counterTask, "OnboardClock") );
/////     printf("finished trying to setRefClkSrc\n");

/////     //printf("About to disable start trig.\n");
/////     //fflush(stdout);
/////     //daq_err_check( DAQmxDisableStartTrig(counterTask) );
/////     fflush(stdout);

/////     // wait for user to start arte counting
    fflush(stdout);
    


    daq_err_check( DAQmxCreateTask( "counter_generation_task",
				    &(counter_generation_task) ));
    daq_err_check( DAQmxCreateTask("counter_count_task",
				   &(counter_count_task) ));

    char co_chan_name[40];
    char ci_chan_name[40];
    char ci_trig_chan_name[40];
    sprintf( co_chan_name, "%s/ctr0", niDev);
    sprintf( ci_chan_name, "%s/ctr1", niDev);
    sprintf( ci_trig_chan_name, "/%s/PFI9", niDev);  // No longer gets used
    
    printf("OK1");fflush(stdout);
    daq_err_check( DAQmxCreateCOPulseChanTicks( counter_generation_task,
						co_chan_name, "", "ai/SampleClock",
						DAQmx_Val_Low, 32,16,16) );
    daq_err_check( DAQmxCfgImplicitTiming( counter_generation_task,
					   DAQmx_Val_ContSamps, 1000) );
    
    daq_err_check( DAQmxCreateCICountEdgesChan( counter_count_task,
						ci_chan_name, "", 
						DAQmx_Val_Rising, 0, DAQmx_Val_CountUp) );
    daq_err_check( DAQmxCfgSampClkTiming( counter_count_task,
					  "Ctr0InternalOutput", 1000.0, DAQmx_Val_Rising,
					  DAQmx_Val_ContSamps, 1000) );
    
    daq_err_check( DAQmxSetRefClkSrc( counter_generation_task, "OnboardClock") );
    daq_err_check( DAQmxSetRefClkSrc( counter_count_task, "OnboardClock") );
    
    printf("abt to start counter_count\n"); fflush(stdout);
    daq_err_check ( DAQmxStartTask( counter_count_task ) );
    printf("abt to start counter_gen\n"); fflush(stdout);
    daq_err_check ( DAQmxStartTask( counter_generation_task ) );
    
    fflush(stdout);
    printf("Reset the master ad clock, and hit Enter here within a half second.\n");
    fflush(stdout);
    // in a separate thread
    char c = 'a';
    while(c != '\n'){
      c = getchar();
    }
    //acquiring = true;
    // end of thread block

    ////printf("About to start counter task.\n");
    ////   daq_err_check( DAQmxStartTask(counterTask) );

    ////printf("finished starting counter task ok.\n");
    ////fflush(stdout);
  }
  
}

void Timer::stop2(){
  
  printf("about to stop counter_gen_task\n"); fflush(stdout);
  daq_err_check( DAQmxStopTask ( counter_generation_task ) );
  printf("finished stopping counter_gen, about to stop counter_count\n"); fflush(stdout);
  daq_err_check( DAQmxStopTask ( counter_count_task ) );
  printf("finished stopping counter_count. About to clear counter_gen\n");fflush(stdout); 
  daq_err_check( DAQmxClearTask ( counter_generation_task ) );
  printf("finished clearing counter_gen, about co clear counter_count\n");fflush(stdout);
  daq_err_check( DAQmxClearTask ( counter_count_task ) );
  printf("finished clearing counter_count\n"); fflush(stdout);
}
