#ifndef SPIKE_DETECTOR_H_
#define SPIKE_DETECTOR_H_

#include "trode.h"
#include "filtered_buffer.h"

enum spikemode_t {THRESHOLD=0, LOCALMAX=1};

class SpikeDetector {

 public:
  
  SpikeDetector();
  virtual ~SpikeDetector();

  void init(Filtered_buffer *fb);

  spikemode_t mode;
  Filtered_buffer * fb;

  // going to try using dynamic memory again
  // little experiment
  // (most dynamic memory was removed from trode, lfp_bank, and filtered_buffer
  // after I felt like later-allocated objects were somehow overwriting parts
  // of earlier ones.  But it doesn't seem like that should have happened, so
  // briefly try again here.

  int spike_cursor;

  rdata_t *thresh;

  int *search_inds;
  int *spike_inds;

  int n_samps_before_spike;
  int n_samps_after_spike;
  uint16_t n_chans;
  uint16_t buffer_n_samps_per_chan;
  uint16_t spike_n_samps_per_chan;

  rdata_t * spike_buffer;

};  

//void init();

void find_spikes(Filtered_buffer *fb);

#endif
