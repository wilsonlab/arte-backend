#ifndef GLOBAL_DEFS_H_
#define GLOBAL_DEFS_H_

#include <stdio.h>
#include <memory.h>
#include <string>
#include <iostream>
#include <map>
#include <limits.h>
#include <stdint.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <memory> // std::shared_ptr
#include <vector>

#include <pthread.h>

// global mutex for printfs
// std::mutex print_mutex;


#define MAX_THREADS 32
#define THREADED_

typedef int16_t rdata_t;
// Don't know why these are giving not-declared errors
const rdata_t RDATA_MIN = INT16_MIN;
const rdata_t RDATA_MAX = INT16_MAX;

typedef uint32_t timestamp_t;
const int TIMESTAMP_MAX = UINT32_MAX;
const int SAMPLE_RATE_HZ = 32000;
const int CLOCK_RATE_HZ = 10000;
const float SAMPLES_PER_TIC = (float)SAMPLE_RATE_HZ / (float)CLOCK_RATE_HZ;

extern pthread_t my_threads[MAX_THREADS];

const int MAX_NAME_STRING_LEN  = 64;
const int MAX_EVENT_STRING_LEN = 256;


//typedef float64 rdata_t;
typedef char name_string_t [MAX_NAME_STRING_LEN];
typedef char event_string_t [MAX_EVENT_STRING_LEN];

const int MAX_NEURAL_DAQS = 16;
const int MAX_TRODES = 64;
const int MAX_LFP_BANKS = 16;
const int MAX_FILTERED_BUFFERS = MAX_TRODES + MAX_LFP_BANKS;

const std::string default_setup_config_filename ("/home/greghale/arte-ephys/conf/arte_setup_default.conf");
const std::string default_session_config_filename ("/home/greghale/arte-ephys/conf/arte_session_default.conf");

const int MAX_NEURAL_DAQ_N_CHANS = 32;
const int MAX_NEURAL_DAQ_N_SAMPS_PER_CHAN = 320;
const int MAX_NEURAL_DAQ_BUFFER = MAX_NEURAL_DAQ_N_CHANS * MAX_NEURAL_DAQ_N_SAMPS_PER_CHAN;


/******* These are all properties of a filtered_buffer now ******/
/* const int MAX_TRODE_N_CHANS = 32;      // 32 chanels */
/* const int MAX_TRODE_BUFFER_LEN = 320; // 3200 samps = about 0.01 seconds */
/* const int MAX_N_INTERMEDIATE_BUFFERS = 10; */
/* const int MAX_TRODE_BUFFER = MAX_TRODE_N_CHANS * MAX_TRODE_BUFFER_LEN * (MAX_N_INTERMEDIATE_BUFFERS + 2); */

/* const int MAX_LFP_BANK_N_CHANS = 32; */
/* const int MAX_LFP_BANK_BUFFER_LEN = 320; */
/* const int MAX_LFP_BANK_N_INTERMEDIATE_BUFFERS = 10; */
/* const int MAX_LFP_BANK_BUFFER = MAX_LFP_BANK_N_CHANS * MAX_LFP_BANK_BUFFER_LEN * MAX_LFP_BANK_N_INTERMEDIATE_BUFFERS; */

const int MAX_FILTERED_BUFFER_N_CHANS = 32;
const int MAX_FILTERED_BUFFER_LEN = 320; //
const int MAX_FILTERED_BUFFER_N_INTERMEDIATE_BUFFERS = 10;
const int MAX_FILTERED_BUFFER_TOTAL_SAMPLE_COUNT_MULTI = 
  MAX_FILTERED_BUFFER_N_CHANS *
  MAX_FILTERED_BUFFER_LEN * MAX_FILTERED_BUFFER_N_INTERMEDIATE_BUFFERS;
const int MAX_FILTERED_BUFFER_TOTAL_SAMPLE_COUNT = MAX_FILTERED_BUFFER_N_CHANS *
  MAX_FILTERED_BUFFER_LEN;

const int MAX_N_SPIKES_PER_BUFFER = 10;

const double NEURAL_DAQ_V_MAX = 10.0;
const double NEURAL_DAQ_V_MIN = -10.0;

const int MAX_FILT_COEFS = MAX_FILTERED_BUFFER_LEN;

const int MAX_SPIKE_N_SAMPS_PER_CHAN = 128;

const int MAX_SPIKE_NET_BUFF_SIZE = 4000; // TODO: FIX THIS WHAT SHOULD THE MAX REALLY BE?
const int MAX_NETCOM_BUFF_SIZE = 4000;
const int MAX_COMMAND_STR_LEN = MAX_NETCOM_BUFF_SIZE - 4;

extern bool daqs_reading; // Daq's read in unison.  No simultaneous mixing between file and card in allowed.
extern bool daqs_writing; // <-- prob won't be used.  Some daq's may write while others don't.

extern uint32_t buffer_count;

// hackish enumeration
typedef uint8_t recordtype_t;
const recordtype_t LFP_BANK_RECORD = 0;
const recordtype_t EVENT_STRING_RECORD = 1;
const recordtype_t SPIKE_RECORD = 2;
const recordtype_t THRESHOLD_FRAME_RECORD = 3;

template <class T>
struct PtrTo{
  typedef std::shared_ptr <T> Type;
  typedef std::vector < std::shared_ptr <T> > ListType;
};



#endif
