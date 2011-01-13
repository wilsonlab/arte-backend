#ifndef FILT_H_
#define FILT_H_

#include <NIDAQmx.h>
#include <string>
#include <boost/property_tree/ptree.hpp>

class Filt{

 public:
  Filt();
  virtual ~Filt();

  void init(boost::property_tree::ptree &filt_pt);

  std::string filt_name;
  std::string type;               // iir or fir.  Determines how to interpret long lists of numerators and denominators
                                  // (iir case, they're treated as second-order sections.  fir case, one long kernel).
  float64 *num_coefs;                     // numerators.
  float64 *denom_coefs;                     // denominators. We don't calculate these.  I get them from matlab filter design tool.

  int order;                      // order of iir, or tap count of fir
  float64 *input_gains;           // must multiply feedforward samples by this to get unity outupt gain in passband (djargonz!)
  
  bool filtfilt;                  // should we collect future data and run the filter backwards?
  int filtfilt_wait_n_buffers;    // how much future 'history' do we collect before backwards filtering?
                                  // (this option is mostly for non-realtime stuff, b/c it requires
                                  //  waiting for ~1 or 2 buffers before giving filtered results.
                                  //  May want to use it in general though, b/c it seems that iir filtering
                                  //  can delay spike peak times by around 0.5 ms.)

  int buffer_mult_of_input;
  int n_samps_per_chan;
  int filt_num_sos;               // derived from order or from numerator, denominator count
  int data_cursor;                

  // filt instances NO NOT have buffers.  This object just keeps parameters for a filter.
  // buffers are kept within invidivual tetrode or eeg_channel objects.
  //float64 *x_buffer;
  //float64 *y_buffer;

};

int rel_pt(int pos, int curs, int buf_len);

void filter_data(float64 *in_buf, Filt filt, int *chans, int n_chans, int in_buf_len, 
		  int out_buf_len, int *u_curs, int *f_curs, int *ff_curs, float64* u_buf, float64 *f_buf, float64 *ff_buf);

void print_array(float64 *buf, int n_chans, int buf_len, int curs);
#endif
