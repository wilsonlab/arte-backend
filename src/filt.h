#ifndef FILT_H_
#define FILT_H_

#include <NIDAQmx.h>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/multi_array.hpp>
#include <memory> //std::shared_ptr
#include "arte_pb.pb.h"
#include "global_defs.h"
#include "neural_daq.h"
#include "a_data_source.h"

#ifndef CBUF
#define CBUF(X,L) (L+X)%L // assumes X >= -L
#endif

class Filt;
typedef std::shared_ptr <Filt> FiltPtr;
typedef std::map <std::string, FiltPtr> FiltList;

class Filt{

 public:
  Filt();
  Filt( ArteFilterOptPb& filt_opt ) ;

  virtual ~Filt();

  void init(boost::property_tree::ptree &filt_pt);

  //  attach_buffers( NeuralVoltageCircBuffer in_buffer, NeuralVoltageCircBuffer out_buffer );

  
  std::string filt_name;
  name_string_t filt_type;               // iir or fir.  Determines how to interpret long lists of numerators and denominators
                                  // (iir case, they're treated as second-order sections.  fir case, one long kernel).
  double num_coefs [MAX_FILT_COEFS];                     // numerators.
  double denom_coefs [MAX_FILT_COEFS];                     // denominators. We don't calculate these.  I get them from matlab filter design tool.

  // Coef array rows are sections, columns are coefficients for that section
  // one SOS has three numerator coefs, three denominator coeffs
  typedef boost::multi_array <double, 2> CoefArray;
  typedef CoefArray::index coeff_array_index;
  CoefArray num_coefs_new;
  CoefArray denom_coefs_new;
  std::vector < double > multipliers;  


  int order;                      // order of iir, or tap count of fir
  double input_gains [MAX_FILT_COEFS];           // must multiply feedforward samples by this to get unity outupt gain in passband (djargonz!)
  
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
  int out_buf_size_bytes;
  int count;

  static FiltList filt_list;
  static void build_filt_list( ArteSetupOptPb & setup_opt );

 private:
  NeuralVoltageCircBuffer *in_buffer, *out_buffer;

};

int rel_pt(int pos, int curs, int buf_len);

void filter_data(rdata_t *in_buf, Filt *filt, neural_daq *nd,  uint16_t *chans, uint16_t n_chans, int in_buf_len, 
		  uint16_t out_buf_len, int *u_curs, int *f_curs, int *ff_curs, rdata_t* u_buf, rdata_t *f_buf, rdata_t *ff_buf);

void print_array(rdata_t *buf, int n_chans, int buf_len, int curs);
#endif
