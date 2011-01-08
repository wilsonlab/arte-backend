#include "util.h"

// general utility functions 

void daq_err_check(int32 error){
  char errBuff[2048] = {'\0'};
  if( DAQmxFailed(error) ){
    std::cout << "Caught a DAQmx error..." << std::endl;
    DAQmxGetExtendedErrorInfo(errBuff,2048);
    std::cout << "util.cpp saw the daqmx error num: " << error << " with message: " << errBuff << std::endl;
  }else{
    std::cout << "No daqmx error." << std::endl;
  }
}

// this is copied from ContinuousAI.c  Not fully sure how it works.
int32 GetTerminalNameWithDevPrefix(TaskHandle taskHandle, const char terminalName[], char triggerName[])
{
  int32 error = 0;
  char chan[256];
  char *slash;

  daq_err_check( DAQmxGetNthTaskChannel(taskHandle, 1, chan, 256) );
  daq_err_check( DAQmxGetPhysicalChanName(taskHandle, chan, chan, 256) );
  if( (slash =strchr(chan,'/')) != NULL){
    *slash = '\0';
    *triggerName++ = '/';
    strcat(strcat(strcpy(triggerName,chan),"/"),terminalName);
  } else
    strcpy (triggerName, terminalName);
}
