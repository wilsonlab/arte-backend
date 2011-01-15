#ifndef GLOBAL_DEFS_H_
#define GLOBAL_DEFS_H_

#include <string>
#include <iostream>
#include <NIDAQmx.h>

const int MAX_TRODE_NAME_LENGTH = 50;
const std::string default_setup_config_filename ("/home/greghale/arte-ephys/conf/arte_setup_default.conf");
const std::string default_session_config_filename ("/home/greghale/arte-ephys/conf/arte_session_default.conf");

const int MAX_NEURAL_DAQ_N_CHANS = 32;
const int MAX_NEURAL_DAQ_N_SAMPS_PER_CHAN = 320;
const int MAX_NEURAL_DAQ_BUFFER = MAX_NEURAL_DAQ_N_CHANS * MAX_NEURAL_DAQ_N_SAMPS_PER_CHAN;

const int MAX_TRODE_N_CHANS = 32;      // 32 chanels
const int MAX_TRODE_BUFFER_LEN = 320; // 3200 samps = about 0.01 seconds
const int MAX_TRODE_BUFFER = MAX_TRODE_N_CHANS * MAX_TRODE_BUFFER_LEN;

struct neural_daq{
  int id;
  std::string dev_name;
  int n_samps_per_buffer;
  int n_chans;
  std::string in_filename;
  std::string raw_dump_filename;
  float64 data_ptr[MAX_NEURAL_DAQ_BUFFER];
  //float64 *data_ptr;
  TaskHandle task_handle;
  int status;
};



#endif
