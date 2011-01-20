#include "timer.h"
#include "util.h"
#include <iostream>
Timer::Timer(string counterID, boolean isMaster){

	niDevice = counterID;
	niDevice = "SET THIS VALUE LATER, THIS VALUE ISNT USED!!!!";
	timerID = -1;

	running = false;
	armed = false;
	master = isMaster;
	isSynced = false;

	initCount = 0;
	countEdge= DAQmx_Val_Rising;
	countDir = DAQmx_Val_CountUp;

	timePort = 4000;

	prevSyncCount=0;
	curSyncCount=0;

}

Timer::~Timer(){
	stopTimer();
}

int Timer::startTimer(){

	if (running){
		std::cout<<"ERROR!!!! The Timer is already running\n";
		return -1;
	}
	if (task!=NULL){
		std::cout<<"ERROR!!!! This timer already has a task\n";
		return -2;
	}

	initDAQSyncTask();

	if (!master)
		rxSyncCount();

	initDAQCounterTask();
	armCounterTask();
	return 0;
}
int Timer::stopTimer(){
	running = false;

	if (task!=0){
		DAQmxStopTask(counterTask);
		DAQmxStopTask(diPulseTask);
		DAQmxClearTask(counterTask);
		DAQmxClearTask(diPulseTask);
		counterTask = NULL;
		diPulseTask = NULL;
	}
	return 0;
}

int  Timer::setInitCount(uint32_t newCount){
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
	//get the count from the counter card and return it
	return 0;
}
uint64_t Timer::getTimestamp(){
	//get the count from the counter card, convert to TS and return it
	return 0;
}

int Timer::initDAQCounterTask(){
	//check to see if the task already exists if so end
	if (task!=0)
		std::cout<<"ERROR!!!! The task is already running, cant setup a new one\n";
	char countDev[] = "/Dev1/ctr0";
	daq_err_check(DAQmxCreateTask("CounterTask", &counterTask));
	daq_err_check(DAQmxCreateCICountEdgesChan(counterTask, countDev, "", countEdge, initCount, countDir);

	return 0;
}

int Timer::initDAQSyncTask(){
	char diTrigLine[] = "/Dev1/port0/line0:7";
	char risingEdge[] = NULL;
	char fallingEdge[]= "/Dev1/port0/line:0:7";

	DAQmxCreateTask("DigitalDetectionTask", @diPulseTask);
	DAQmxCreateDIChan(diPulseTask, diTrigLine, "", DAQmx_Val_ChanPerLine);
	DAQmxCfgChangeDetectionTiming(diPulseTask, risingEdge, fallingEdge, DAQmx_Val_ContSamps,1);
	DAQmxRegisterSignalEvent(task, DAQmx_Val_ChangeDetectionEvent,0,digitalChangeCallback, NULL);

	return 0;
}

int Timer::armCounterTask(){
	char ctrTrgChan[] = "/Dev1/PFI14";
	daq_err_check(DAQmxSetArmStartTrigType(counterTask, DAQmx_Val_DigEdge));
	daq_err_check(DAQmxSetDigEdgeArmStartTrigSrc(counterTask, ctrTrgChan));
	daq_err_check(DAQmxSetDigEdgeArmStartTrigEdge(counterTask, DAQmx_Val_Rising));

	return 0;
}

int32 CVICALLBACK digitalChangeCallback(TaskHandle taskHandle, int32 signalID, void *callbackData){
	prevSyncCount = curSyncCount;
	if (master){

		curSyncCount=getCount();
		if (prevSyncCount==0)
			return -1;

		uint32_t nextSyncCount = curSyncCount + (curSyncCount-prevSyncCount);
		txSyncCount(nextSyncCount, N_MASTER_SYNC_TX);
	}
	else
		txSyncCount(curSyncCount, N_SLAVE_SYNC_TX);

	return 0;
}
int Timer::txSyncCount(uint32_t syncCount, int nPackets)
{
	std::cout<<"Warning: Not TXing packets!!!!!!!!!!!\n";
	return 0;
}
int Timer::rxSyncCount()
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



int Timer::setTimerID(int8_t newId){
	timerID = newId;
	return 0;
}
int8_t Timer::getTimerID(){
	return timerID;
}

