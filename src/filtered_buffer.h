#ifndef FILTERED_BUFFER_H_
#define FILTERED_BUFFER_H_

#include "global_defs.h"
#include <string>
#include "filt.h"
#include <boost/property_tree/ptree.hpp>
#include <map>
#include <stdint.h>
#include "util.h"
#define CBUF(X,L) (L+X)%L // assumes X >= -L

class Filtered_buffer{

 public:
  Filtered_buffer();
  virtual ~Filtered_buffer();
  
  uint16_t name; // currently unused.  trode just gets a ptr to this object
  uint16_t n_chans;
  uint16_t daq_id;
  uint16_t stream_n_samps_per_chan;
  uint16_t stream_n_chans;
  Filt my_filt;
  int buffer_mult_of_input;
  uint16_t buf_len;    // samps per chan of output buffer
  size_t buff_size_bytes;
  uint16_t channels[MAX_FILTERED_BUFFER_N_CHANS];
  neural_daq *my_daq;

  name_string_t buffer_dump_filename;
  // make 2 files, one for unfiltered data, the other for filtered
  // we'll append ".unfilt" and ".filt" to the filenames
  FILE *buffer_dump_file_uf;
  FILE *buffer_dump_file_f;

  rdata_t **ptr_to_raw_stream;
  
  rdata_t f_buf  [MAX_FILTERED_BUFFER_TOTAL_SAMPLE_COUNT];
  rdata_t u_buf  [MAX_FILTERED_BUFFER_TOTAL_SAMPLE_COUNT_MULTI];
  rdata_t ff_buf [MAX_FILTERED_BUFFER_TOTAL_SAMPLE_COUNT];

  int u_curs;   // *_curs is the index into the circular buffer for
  int f_curs;   // the first valid samp after filtering is done. See 
  int ff_curs;  // Filtered_buffer::write_buffers for usage example

  void print_options();

  void print_buffers(int chan_lim);

  void init(boost::property_tree::ptree&,
	    boost::property_tree::ptree&,
	    int min_samps);

  void init_files(std::string &filename);
  void write_buffers();
  void finalize_files();
  bool finalize_done;
  uint32_t file_buffer_count;
  
};

//void write_files();

extern std::map<uint16_t, Filtered_buffer> filtered_buffer_map;
extern Filtered_buffer * filtered_buffer_array;

extern std::map <std::string, Filt> filt_map;
extern std::map <int, neural_daq>   neural_daq_map;
extern neural_daq * neural_daq_array;

extern int n_neural_daqs;

#endif
