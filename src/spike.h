#ifndef SPIKE_H_
#define SPIKE_H_

// Eventualy, we want the protocol buffer itself to 
// be the only 'spike' class, so that we don't have to
// be packing/unpacking, synchronizing another class
// with it.
//
// For now, I just want to make a drop-in replacement for
// the old spike_net_t that is mostly compatible with spike_net_t's
// but can also produce protobufs, so that I can add protobuf
// outputs from backend.  Recommend that new spike accumulators
// don't use this class, just use ArtePb instead, and hope that
// backend is producing protobufs soon.

#include "stdint.h"
#include "global_defs.h"
#include "arte_pb.pb.h"
#include "datapacket.h"

class Spike {
 public:

  Spike(uint16_t name, int n_chans, int n_samps, int samp_n_bytes,
	int16_t gains[], rdata_t thresh[], 
	uint16_t trig_ind );

  // Try to be drop_in replacement for spike_net_t
  timestamp_t ts;
  uint16_t name;
  uint16_t n_chans;
  uint16_t n_samps_per_chan;
  uint16_t samp_n_bytes;
  rdata_t data[MAX_FILTERED_BUFFER_TOTAL_SAMPLE_COUNT];
  int16_t  gains[MAX_FILTERED_BUFFER_N_CHANS];
  rdata_t  thresh[MAX_FILTERED_BUFFER_N_CHANS];
  uint16_t trig_ind;
  uint32_t seq_num;

  // Add PB-maker, and spike_net_t maker
  ArtePb & get_pb();
  void get_spike_net (spike_net_t *sn);

 private:
  // These will only be built up when they're asked for
  ArtePb my_pb;
  ArteSpikePb *my_spike;
  spike_net_t my_spike_net;

};

#endif
