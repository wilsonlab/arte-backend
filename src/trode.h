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
    double *thresholds;
    int daq_id;
    int stream_n_samps_per_chan;
    int stream_n_chan;
    Filt my_filt;
    std::string spike_mode;
    int buffer_mult_of_input;
    int *channels;
    int samps_before_trig;
    int samps_after_trig;
    int n_samps_per_spike;
  } trode_opt;

  int init(boost::property_tree::ptree &trode_pt, boost::property_tree::ptree &default_pt,
	   std::map<int, neural_daq> &neural_daq_map, std::map<std::string, Filt> &filt_map);

  void print_options(void);
  void print_buffers(void);
  void print_spikes(void);
  //add_data(prt_to_array, n_chans, n_samps_per_chan, time_at_beginning_of_buffer)
  //void process_data();
  // NB: we'll prob use the fn's trode_filter_data & trode_process_data instead of this method
  // to save overhead.  But let's keep this member fn and implement it so we can compare the execution time

  std::string trode_name;
  std::string filt_name;
  float64 *ptr_to_raw_stream;
  int buffer_mult_of_input;     // how many input buffers do we keep for this trode?
  float64 *test;
  float64 *u_buf;                // unfiltered data buffer 
  float64 *f_buf;                // filtered data buffer
  float64 *ff_buf;              // filtfiltered data buffer
  int u_curs;
  int f_curs;
  int ff_curs;
  int u_curs_time;
  int f_curs_time;

  int n_chans;                  // num of chans for this trode
  int n_samps_per_chan;         // num of samps per chan (derived from samps_after_trig & samps_before_chan)
  int *channels;                // ptr is to 32-chan signal.  Which rows belong to this trode?

  int daq_id;                   // id number of neural daq card that this trode sees
  //neural_daq_card daq_card;   // data struct of that daq card
  //can't do that b/c neural_daq_card defined in arteopt, which includes trode.h
  int stream_samps_per_chan;    // how many samps come in with each raw buffer?
  int stream_n_chan;            // how many chans come in with each raw buffer? (this will almost always be 32)

  //Filt my_filt;                 // a Filt instance to hold filter parameters

  std::string spike_mode;       // instructions for centering spikes.  'thresh' means center spikes on xing. 'peak' means on next local max
  float64 *thresholds;          // array of threshold voltages, 1 per chan
  int samps_before_trig;        // how many samps to collect before trig ind
  int samps_after_trig;         // hom many samps to collect after trig ind (total samps is after + before + 1

  //float64 *win_heights;       // array of display ranges. I know, we should separate this data structure from
                                // the visualization, but it's hugely convenient to store this tetrode-by-tetrode data here
                                // ACTUALLY nevermind.  we'll store this where it actually belongs.  A trode_disp_map in
                                // the visualization program

  int tmp;

};

extern std::map<std::string, Trode> trode_map;

void trode_filter_data(Trode *); // trode_filter_data(trode_to_prosess, raw buffer, timestamp at buffer start)
void trode_process_data(Trode *); // only need access to this trode's buffers (filtered data is there)

#endif
