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

