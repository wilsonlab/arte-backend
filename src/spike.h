#include "global_defs.h"
#include <NIDAQmx.h>

// a struct to pass spike data
// spike.source_trode      which trode did spike come from (id num?  or name string?)
// spike.n_chans           how many chans in the source trode
// spike.n_samps_per_chan  how many sampls in a channel for 1 spike
// spike.tt                float64 array [n_chans * n_samps_per_chan], grouped so all samps from 1 chan are adjacent
// spike.parms             float64 array [number of params (eg chan1 spike height, chan2 hgt, chan3 hgt, mean hgt, t_peak, spike_wid)]

// is it Ok to have two arrays of unknown length in the same struct?  seem to remember this being illegal

struct spike{
  char source_trode[MAX_TRODE_NAME_LENGTH];
  int n_chans;
  int n_samps_per_chan;
  float64 *tt;
  float64 *parms;
};
