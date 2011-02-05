#include <NIDAQmx.h>
#include <iostream>
#include "nichk.h"

static TaskHandle	t1,t2;

uInt32 prev=0, curr=0, next=0, diff=0;
int32 err=0;
int n=0;

int32 CVICALLBACK ChangeDetectionCallback(TaskHandle t1, int32 signalID, void *callbackData);
void Cleanup (void);

int main(void){
	int32       error=0;
	char        errBuff[2048]={'\0'};

	//prepare the digital trigger task
	ECmx (DAQmxCreateTask("",&t1));
	ECmx (DAQmxCreateDIChan(t1,"Dev1/port0/line0:7","",DAQmx_Val_ChanPerLine));
	ECmx (DAQmxCfgChangeDetectionTiming(t1,"Dev1/port0/line0:7",NULL,DAQmx_Val_ContSamps,1));
	ECmx (DAQmxRegisterSignalEvent(t1,DAQmx_Val_ChangeDetectionEvent,0,ChangeDetectionCallback,NULL));
	ECmx (DAQmxStartTask(t1));

	//prepare the counter task
	ECmx (DAQmxCreateTask("",&t2));
	ECmx (DAQmxCreateCICountEdgesChan(t2,"Dev1/ctr1","",DAQmx_Val_Rising,0,DAQmx_Val_CountUp));
        ECmx (DAQmxStartTask(t2));

	//wait until the user kills the program
	getchar();
	Cleanup();
}

int32 CVICALLBACK ChangeDetectionCallback(TaskHandle t1, int32 signalID, void *callbackData){
	n++;
	uInt32 count;
        ECmx (DAQmxReadCounterScalarU32(t2,10.0,&count,NULL));

	prev = curr;
	curr = count;
	diff = curr-prev;
	diff = 5000;
		
	err = next-curr;
	next = curr+diff;
	if (n<5)
		return 0;
	if (err!=0){
		std::cout<<"\tActual: "<<curr;
		std::cout<<"\tErr: "<<err;
		std::cout<<"\tLoop Number:"<<n<<std::endl;
		std::cout<<"Prediction: "<<next;
	}
	return 0;
}
void Cleanup (void){
	if( t1!=0 ) 
	{
		DAQmxStopTask(t1);
		DAQmxClearTask(t1);
		DAQmxStopTask(t2);
		DAQmxClearTask(t2);
		t1 = 0;
		t2 = 0;
	}
}

