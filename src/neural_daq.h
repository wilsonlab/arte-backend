#ifndef NEURAL_DAQ_H_
#define NEURAL_DAQ_H_

#include "util.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include "global_defs.h"
#include <map>

extern int master_id;
extern int32 buffer_size;
extern int buffer_count;

void neural_daq_init(boost::property_tree::ptree &setup_ptree);

void neural_daq_start_all(void);

void neural_daq_stop_all(void);

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNSamplesEventType, uInt32 nSamples, void *callbackData);

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

neural_daq find_neural_daq_by_taskhandle(TaskHandle taskHandle);

void print_neural_daq(neural_daq nd);
void print_buffer(neural_daq *ndp, int row_lim, int row_length, int col_lim);

#endif
