#ifndef NEURAL_DAQ_H_
#define NEURAL_DAQ_H_

#include "util.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include "global_defs.h"
#include <map>

extern int master_id;
extern int32 buffer_count;
extern int32 buffer_size;

void neural_daq_init(boost::property_tree::ptree &setup_ptree);

void neural_daq_start_all(void);

void neural_daq_stop_all(void);

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNSamplesEventType, uInt32 nSamples, void *callbackData);

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

#endif
