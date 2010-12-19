// a struct to pass spike data
// spike.source_trode      which trode did spike come from (id num?  or name string?)
// spike.n_chans           how many chans in the source trode
// spike.n_samps_per_chan  how many sampls in a channel for 1 spike
// spike.tt                float64 array [n_chans * n_samps_per_chan], grouped so all samps from 1 chan are adjacent

struct spike{
  char source_trode[MAX_TRODE_NAME_LENGTH];
