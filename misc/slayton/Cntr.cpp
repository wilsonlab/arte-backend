#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main(void)
{
	int         error=0;
	TaskHandle  taskHandle=0;
	uInt32      data;
	char        errBuff[2048]={'\0'};

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateCICountEdgesChan(taskHandle,"Dev1/ctr1","",DAQmx_Val_Rising,0,DAQmx_Val_CountUp));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/

// 	DAQmxSetArmStartTrigType(taskHandle, DAQmx_Val_DigEdge);
// 	DAQmxSetDigEdgeArmStartTrigSrc(taskHandle, "/Dev1/PFI7");
// 	DAQmxSetDigEdgeArmStartTrigEdge(taskHandle, DAQmx_Val_Rising);
// 	DAQmxErrChk (DAQmxStartTask(taskHandle));
	
	DAQmxErrChk (DAQmxDisableStartTrig(taskHandle));
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	printf("Continuously polling. Press Ctrl+C to interrupt\n");
	while( 1 ) {
		/*********************************************/
		// DAQmx Read Code
		/*********************************************/
		DAQmxErrChk (DAQmxReadCounterScalarU32(taskHandle,10.0,&data,NULL));

		printf("\rCount: %d",data);
		fflush(stdout);
	}

Error:
	puts("");
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
	return 0;
}
