#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int32 CVICALLBACK DoneCallback(TaskHandle th1, int32 status, void *callbackData);
	int slow = 1;
	int fast = 10000;


int main(void)
{
	int         error=0;
	TaskHandle  th1=0;
	TaskHandle  th2=0;
	char        errBuff[2048]={'\0'};

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&th1));
	DAQmxErrChk (DAQmxCreateTask("",&th2));
	
	DAQmxErrChk (DAQmxCreateCOPulseChanFreq(th1, "Dev1/ctr0", "", DAQmx_Val_Hz, DAQmx_Val_Low, 0.0, slow, 0.50));
	DAQmxErrChk (DAQmxCreateCOPulseChanFreq(th2, "Dev1/freqout", "", DAQmx_Val_Hz, DAQmx_Val_Low, 0.0, fast, 0.50));

	DAQmxErrChk (DAQmxCfgImplicitTiming(th1,DAQmx_Val_ContSamps,1000));
	DAQmxErrChk (DAQmxCfgImplicitTiming(th2,DAQmx_Val_ContSamps,1000));

	DAQmxErrChk (DAQmxRegisterDoneEvent(th1,0,DoneCallback,NULL));
	DAQmxErrChk (DAQmxRegisterDoneEvent(th2,0,DoneCallback,NULL));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(th1));
	DAQmxErrChk (DAQmxStartTask(th2));

	printf("Generating pulse train. Press Enter to interrupt\n");
	getchar();

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( th1!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(th1);
		DAQmxStopTask(th2);
		DAQmxClearTask(th1);
		DAQmxClearTask(th2);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
	return 0;
}

int32 CVICALLBACK DoneCallback(TaskHandle th, int32 status, void *callbackData)
{
	int32   error=0;
	char    errBuff[2048]={'\0'};

	// Check to see if an error stopped the task.
	DAQmxErrChk (status);

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		DAQmxClearTask(th);
		printf("DAQmx Error: %s\n",errBuff);
	}
	return 0;
}
