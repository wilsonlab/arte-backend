#ifndef TRODE_H_
#define TRODE_H_

#include "global_defs.h"
#include <string>
#include "filt.h"
#include <boost/property_tree/ptree.hpp>
#include <map>
#include <vector>
#include <stdint.h>

#include "spike.h"
#include "filtered_buffer.h"
#include "netcom.h"
#include "arte_pb.pb.h"

extern neural_daq      * neural_daq_array;
extern Filtered_buffer * filtered_buffer_array;
extern std::map< std::string, Filt > filt_map;
extern std::vector<NetCom> netcom_vector;

class Trode{

 public:
  Trode();
  Trode( ArteSessionOptPb &session_pb );
  virtual ~Trode();
  
  uint16_t name;

  uint16_t n_chans;
  rdata_t thresholds[MAX_FILTERED_BUFFER_N_CHANS];
  uint16_t  gains[MAX_FILTERED_BUFFER_N_CHANS];
  name_string_t spike_mode;
  uint16_t samps_before_trig;
  uint16_t samps_after_trig;
  uint16_t n_samps_per_spike;
  uint16_t refractory_period_samps;
  uint16_t refractory_period_tics;
  uint32_t next_ok_spike_ts;
  uint32_t total_spikes;
  
  Filtered_buffer *my_buffer;

  neural_daq *my_daq;

  //SpikeDetector my_spike_detector;  // <-- No such thing yet :)

  spike_net_t spike_array[MAX_N_SPIKES_PER_BUFFER];
  ArtePb my_arte_pb;
  ArteSpikePb *my_spike_pb;
  ArteTrodeOptPb my_opt_pb;

  NetCom * my_netcom;
  NetComDat my_netcomdat;
  bool has_sockfd;

  //for debugging
  int *n_times_checked;

  void init2(boost::property_tree::ptree &trode_pt,
	    boost::property_tree::ptree &default_pt,
	    Filtered_buffer * filtered_buffer_curs);

  void setup_spike_array();
  void setup_pb();
  void set_thresh_uv_one_thresh_n_chans(int16_t uv_thresh, int *chan_array, int n_chans_to_set);
  void set_thresh_uv_n_thresh_n_chans(int16_t *uv_thresh_array, int *chan_array, int n_chans_to_set);
  void set_thresh_uv_one_thresh_all_chans(int16_t uv_thresh);

  void print_options();
  void print_buffers(int chan_lim, int samp_lim); // this should be handled by my_buffer, not trode
  void print_spikes();
  void end_acquisition();

  void spike_to_pb(spike_net_t *spike);

};

int find_spikes(Trode *t);

void spike_to_disk(spike_net_t *spike);
void spike_to_net(spike_net_t *spike, Trode *t);

extern std::map<uint16_t, Trode> trode_map;
extern Trode * trode_array;

void *trode_filter_data(void *); // trode_filter_data(void * trode_to_prosess) void* will be cast to Trode* in fn definition
//void trode_process_data(Trode *); // only need access to this trode's buffers (filtered data is there)

#endif
