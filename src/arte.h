// this will be the header file atop the main loop
// global variables declared here
#include <string>

#include "global_defs.h"
#include "neural_daq.h"
#include "filtered_buffer.h"
#include "timer.h"
#include "trode.h"
#include "lfp_bank.h"


neural_daq * neural_daq_array;
Filtered_buffer * filtered_buffer_array;
Trode * trode_array;
Lfp_bank * lfp_bank_array;

int n_neural_daqs = 0;
int n_filtered_buffers = 0;
int n_trodes = 0;
int n_lfp_banks = 0;
