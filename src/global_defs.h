#ifndef GLOBAL_DEFS_H_
#define GLOBAL_DEFS_H_

#include <string>
#include <iostream>

const int MAX_TRODE_NAME_LENGTH = 50;
const std::string default_setup_config_filename ("/home/greghale/arte-ephys/conf/arte_setup_default.conf");
const std::string default_session_config_filename ("/home/greghale/arte-ephys/conf/arte_session_default.conf");

struct neural_daq{
  int id;
  std::string dev_name;
  int n_samps_per_buffer;
  int n_chans;
  std::string in_filename;
  std::string raw_dump_filename;
  double *data_ptr;
};



#endif
