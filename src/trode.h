#ifndef TRODE_H_
#define TRODE_H_

#include "global_defs.h"
#include <string>
#include "filt.h"
#include <boost/property_tree/ptree.hpp>
#include <map>

class Trode{

 public:
  Trode();
  virtual ~Trode();

  struct trode_opt_struct{
    std::string trode_name;
    std::string filt_name;
    int n_chans;
    double thresholds[MAX_TRODE_N_CHANS];
    int daq_id;
    int stream_n_samps_per_chan;
    int stream_n_chan;
    Filt my_filt;
    std::string spike_mode;
    int buffer_mult_of_input;
    int buf_len;
    int buf_size_bytes;
    int channels[MAX_TRODE_N_CHANS];
    int samps_before_trig;
    int samps_after_trig;
    int n_samps_per_spike;
  } trode_opt;

  neural_daq *my_daq;

  int init(boost::property_tree::ptree &trode_pt, boost::property_tree::ptree &default_pt,
	   std::map<int, neural_daq> &neural_daq_map, std::map<std::string, Filt> &filt_map);

  void print_options(void);
  void print_buffers(int chan_lim, int samp_lim);
  void print_spikes(void);


  float64 **ptr_to_raw_stream;
  float64 u_buf [MAX_TRODE_BUFFER];                // unfiltered data buffer 
  float64 f_buf [MAX_TRODE_BUFFER];                // filtered data buffer
  float64 ff_buf [MAX_TRODE_BUFFER];             // filtfiltered data buffer
  int u_curs;
  int f_curs;
  int ff_curs;
  int u_curs_time;
  int f_curs_time;

};

extern std::map<std::string, Trode> trode_map;

void *trode_filter_data(void *); // trode_filter_data(trode_to_prosess, raw buffer, timestamp at buffer start)
void trode_process_data(Trode *); // only need access to this trode's buffers (filtered data is there)

#endif
