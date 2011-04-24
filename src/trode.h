#ifndef TRODE_H_
#define TRODE_H_

#include "global_defs.h"
#include <string>
#include "filt.h"
#include <boost/property_tree/ptree.hpp>
#include <map>
#include <stdint.h>
#include "filtered_buffer.h"

extern neural_daq      * neural_daq_array;
extern Filtered_buffer * filtered_buffer_array;
extern std::map< std::string, Filt > filt_map;

class Trode{

 public:
  Trode();
  virtual ~Trode();
  
/*    struct trode_opt_struct{  */
/*      uint16_t trode_name;  */
/*      name_string_t filt_name;  */
/*      uint16_t n_chans;  */
/*      rdata_t thresholds[MAX_TRODE_N_CHANS];  */
/*      uint16_t daq_id;  */
/*      uint16_t stream_n_samps_per_chan;  */
/*      uint16_t stream_n_chan;  */
/*      Filt my_filt;  */
/*      name_string_t spike_mode;  */
/*      int buffer_mult_of_input;  */
/*      int buf_len;  */
/*      int buf_size_bytes;  */
/*      uint16_t channels[MAX_TRODE_N_CHANS];  */
/*      uint16_t samps_before_trig;  */
/*      uint16_t samps_after_trig;  */
/*      uint16_t n_samps_per_spike;  */
/*    } trode_opt;  */
  
  uint16_t name;

  uint16_t n_chans;
  rdata_t thresholds[MAX_FILTERED_BUFFER_N_CHANS];
  name_string_t spike_mode;
  uint16_t samps_before_trig;
  uint16_t samps_after_trig;
  uint16_t n_samps_per_spike;
  
  Filtered_buffer *my_buffer;

  neural_daq *my_daq;

  int init(boost::property_tree::ptree &trode_pt, 
	   boost::property_tree::ptree &default_pt,
	   std::map<int, neural_daq> &neural_daq_map, 
	   std::map<std::string, Filt> &filt_map);

  void init2(boost::property_tree::ptree &trode_pt,
	    boost::property_tree::ptree &default_pt,
	    Filtered_buffer * filtered_buffer_curs);

  void print_options();
  void print_buffers(int chan_lim, int samp_lim); // this should be handled by my_buffer, not trode
  void print_spikes();


/*   rdata_t **ptr_to_raw_stream; */
/*   rdata_t u_buf [MAX_TRODE_BUFFER];                // unfiltered data buffer  */
/*   rdata_t f_buf [MAX_TRODE_BUFFER];                // filtered data buffer */
/*   rdata_t ff_buf [MAX_TRODE_BUFFER];             // filtfiltered data buffer */
/*   int u_curs; */
/*   int f_curs; */
/*   int ff_curs; */
/*   int u_curs_time; */
/*   int f_curs_time; */

/*   FILE *buffer_dump_file; */

};

extern std::map<uint16_t, Trode> trode_map;
extern Trode * trode_array;

void *trode_filter_data(void *); // trode_filter_data(void * trode_to_prosess) void* will be cast to Trode* in fn definition
//void trode_process_data(Trode *); // only need access to this trode's buffers (filtered data is there)

#endif
