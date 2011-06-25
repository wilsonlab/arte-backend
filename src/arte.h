// this will be the header file atop the main loop
// global variables declared here
#include <string>

#include "global_defs.h"
#include "neural_daq.h"
#include "filtered_buffer.h"
#include "timer.h"
#include "trode.h"
#include "lfp_bank.h"

// Do we really need all these arrays and counters to be declared here?

extern neural_daq * neural_daq_array;
extern Filtered_buffer * filtered_buffer_array;
extern Trode * trode_array;
extern Lfp_bank * lfp_bank_array;

extern int n_neural_daqs;
extern int n_filtered_buffers;
extern int n_trodes;
extern int n_lfp_banks;

extern FILE *main_file;
