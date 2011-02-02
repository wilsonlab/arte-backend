#include "util.h"
#include "timer.h"

// general utility functions 

void daq_err_check(int32 error){
  char errBuff[2048];
  if( DAQmxFailed(error) ){
    std::cout << "Caught a DAQmx error..." << std::endl;
    DAQmxGetExtendedErrorInfo(errBuff,2048);
    std::cout << "util.cpp saw the daqmx error num: " << error << " with message: " << errBuff << std::endl;
  }else{
    //    std::cout << "No daqmx error." << std::endl;
  }
}

void ECmx(int32 error){
  char errBuff[2048];
  if( DAQmxFailed(error) ){
    std::cout << "Caught a DAQmx error..." << std::endl;
    DAQmxGetExtendedErrorInfo(errBuff,2048);
    std::cout << "util.cpp saw the daqmx error num: " << error << " with 
messag$
  }else{
    //    std::cout << "No daqmx error." << std::endl;
  }
}


void daq_err_check(int32 error, TaskHandle *task_handle_array, int n_tasks){
  for (int n = 0; n < n_tasks; n++){
    std::cout << "Checking for errors while stopping task " << n << std::endl;
    daq_err_check( DAQmxStopTask(task_handle_array[n]) );
  }
  std::cout << "The original error check:" << std::endl;
  daq_err_check(error);
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
  return 0;
}

FILE *try_fopen( const char *filename, const char *mode){
  FILE *fp = fopen( filename, mode );
  if(fp == NULL){
    printf("From try_fopen: file error while opening %s .  Exiting now.", filename);
    exit(1);
  }
  return fp;
}
