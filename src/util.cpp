#include "util.h"
#include <stdint.h>
#include <cstring>
#include <iostream>
#include <assert.h>

//defines path relative to home
std::string appendhome( std::string const & filename ) 
{
  char * val = getenv("HOME");
  assert (val != NULL); 
    return std::string(val) + "/" + "filename";
}

int main (int argc, char *argv[]){

std::cout << "output is :" << appendhome(filename) << std::endl;
return 0;
}



rdata_t ftor(double *f_val){
  return  (rdata_t)  ( (*f_val)/NEURAL_DAQ_V_MAX * RDATA_MAX); // assume n_daq range and rdata range both symmetric around 0
  // properway would be:  (  rdata_min + (f_val - n_daq_min)/(n_daq_max - n_daq_min)  * (rdata_max - rdata_min))
}

void ftor_a(double *f_val, rdata_t *r_value, int n_elem){
  for(int i = 0; i < n_elem; i++){
    r_value[i] = ftor(f_val+i);
  }
}

// general utility functions 

void daq_err_check(int32 error){
  char errBuff[2048];
  if( DAQmxFailed(error) ){
    std::cout << "Caught a DAQmx error..." << std::endl;
    fflush(stdout);
    DAQmxGetExtendedErrorInfo(errBuff,2048);
    std::cout << "util.cpp saw the daqmx error num: " << error << " with message: " << errBuff << std::endl;
    fflush(stdout);
  }else{
    //    std::cout << "No daqmx error." << std::endl;
  }
}

void ECmx(int32 error){
  char errBuff[2048];
  if( DAQmxFailed(error) ){
    std::cout << "Caught a DAQmx error..." << std::endl;
    fflush(stdout);
    DAQmxGetExtendedErrorInfo(errBuff,2048);
    std::cout << "util.cpp saw the daqmx error num: " << error << " with message: " << errBuff << std::endl;
    fflush(stdout);
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

void try_fclose( FILE *the_file ){
  int r;
  if( the_file == NULL ){
    printf("Tried to close a file that isn't open\n");
  } else {
    r = fclose( the_file );
  }
  if(r > 0){
    printf("Got an error closing a file.\n");
  }
}
