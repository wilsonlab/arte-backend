#ifndef NEURAL_DAQ_H_
#define NEURAL_DAQ_H_

#include "util.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include "global_defs.h"
#include <map>
#include "timer.h"
#include <NIDAQmx.h>

extern int master_id;
extern int32_t buffer_size;


struct neural_daq{
  int id;
  name_string_t dev_name;
  uint16_t n_samps_per_buffer;
  uint16_t n_chans;
  name_string_t in_filename;
  FILE *in_file;
  name_string_t raw_dump_filename;
  FILE *out_file;
  rdata_t data_buffer[MAX_NEURAL_DAQ_BUFFER];
  rdata_t *data_ptr;
  uint32_t buffer_timestamp;
  TaskHandle task_handle;
  int total_samp_count;
  int size_bytes;
  int status;
  double buffer_time_interval;  // inter-buffer-interval (sec)
  uint32_t daq_buffer_count;
  uint32_t this_buffer;

  TaskHandle counter_generation_task;
  TaskHandle counter_count_task;
};


void neural_daq_init(boost::property_tree::ptree &setup_ptree);

void neural_daq_start_all(void);

void neural_daq_stop_all(void);

void read_data_from_file(void);

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, 
				 int32 everyNSamplesEventType, 
				 uInt32 nSamples, void *callbackData);

void do_cycle(timestamp_t this_cycle_time);
void *process_cycle(void *thread_data);
void *process_daq(void *thread_data);
void *process_trode(void *thread_data);
void *process_lfp_bank(void *thread_data);

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, 
			       void *callbackData);

neural_daq find_neural_daq_by_taskhandle(TaskHandle taskHandle);

void print_neural_daq(neural_daq nd);

void print_buffer(neural_daq *ndp, int row_lim, 
		  int row_length, int col_lim);

void init_files(void);
void finalize_files(void);

#endif
