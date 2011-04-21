#ifndef LFP_BANK_H_
#define LFP_BANK_H_

#include "global_defs.h"
#include <string>
#include "filt.h"
#include <boost/property_tree/ptree.hpp>
#include <map>
#include <stdint.h>
#include "util.h"

class Lfp_bank{

 public:
  Lfp_bank();
  virtual ~Lfp_bank();

  uint16_t lfp_bank_name;
  name_string_t filt_name;
  uint16_t n_chans;
  
/*   uint16_t daq_id; */
/*   uint16_t stream_n_samps_per_chan; */
/*   uint16_t stream_n_chans; */
/*   Filt my_filt; */
/*   int buffer_mult_of_input; */
/*   uint16_t buf_len; // how many samps in 1 chan after downsampling */
/*   int buf_size_bytes; */
/*   uint16_t channels[MAX_LFP_BANK_N_CHANS]; */

  uint16_t keep_nth_sample;

  //  neural_daq *my_daq;

  //std::string buffer_dump_filename;
  

  name_string_t lfp_bank_dump_filename;


/*   rdata_t **ptr_to_raw_stream; */
/*   rdata_t u_buf  [MAX_LFP_BANK_BUFFER];  //unfiltered buffer  */
/*                                          //(later elemnts, intermediate filt levels) */
/*   rdata_t f_buf  [MAX_LFP_BANK_BUFFER];  //filtered buffer */
/*   rdata_t ff_buf [MAX_LFP_BANK_BUFFER];  //filtfilted buffer */
/*   rdata_t d_buf  [MAX_LFP_BANK_BUFFER];  //downsampled buffer */

/*   FILE *buffer_dump_file; */


  FILE *lfp_bank_dump_file;

  int init(boost::property_tree::ptree &lfp_bank_ptree,
	   std::map<int, neural_daq> &neural_daq_map,
	   std::map<std::string, Filt> &filt_map);

/*   int u_curs; */
/*   int f_curs; */
/*   int ff_curs; */
/*   int u_curs_time; */
/*   int f_curs_time; */

  void print_options(void);
  void print_buffers(int chan_lim);  // print the input streams and decimated output

};

extern std::map<uint16_t, Lfp_bank> lfp_bank_map;

void *lfp_bank_filter_data(void *); // modeled after trode_filter_data
// gets passed a pointffer that will be cast to Lfp_bank*  (void input b/c may be getting pointers to a base class of Lfp_bank and Trode)
// if we make those classes derived from a common one in the future.
void lfp_bank_write_record(void *); // 


#endif
