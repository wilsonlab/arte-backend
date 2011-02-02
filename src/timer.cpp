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
	setDevStrs("/Dev1/", "/Dev1/ctr0", "/Dev1/PFI2", "/Dev1/port0/line0:7");

	counterTask = 0;
	diPulseTask = 0;

	std::cout<<"Timer::Timer end"<<std::endl;


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
uint64_t Timer::getTimestamp(){
	std::cout<<"Timer::getTimestamp() called!"<<std::endl;
	//get the count from the counter card, convert to TS and return it
	return 0;
}

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
	DAQmxRegisterSignalEvent(diPulseTask,DAQmx_Val_ChangeDetectionEvent,0,timerDigTrigCallback, 
this);

	return 0;
}

int Timer::armCounterTask(){
	std::cout<<"Timer::armCounterTask() called!"<<std::endl;
	std::cout<<"\tsetting the counter to trigger off of:"<<niArm<<std::endl;
	daq_err_check(DAQmxSetArmStartTrigType(counterTask, DAQmx_Val_DigEdge));
	daq_err_check(DAQmxSetDigEdgeArmStartTrigSrc(counterTask, niArm));
	daq_err_check(DAQmxSetDigEdgeArmStartTrigEdge(counterTask, DAQmx_Val_Rising));
	

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
	txDat = NetCom::initUdpTx(host.c_str(), port);	
}
void Timer::initUdpRx(std::string ip, int port){
	rxDat = NetCom::initUdpRx(host.c_str(), port);
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

