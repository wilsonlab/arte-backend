#ifndef SPIKE_DETECTOR_H_
#define SPIKE_DETECTOR_H_

#include "trode.h"

enum spikemode_t {THRESHOLD=0, LOCALMAX=1};

class SpikeDetector {

 public:
  
  spikemode_t mode;
  int n_chans;

};  

void find_spikes(Trode *trode);

#endif
