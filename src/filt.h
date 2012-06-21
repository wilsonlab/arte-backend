#ifndef FILT_H_
#define FILT_H_

#include <NIDAQmx.h>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/multi_array.hpp>
#include <memory> //std::shared_ptr
#include "datapacket.h"
#include "arte_pb.pb.h"
#include "global_defs.h"
#include "neural_daq.h"
#include "a_data_source.h"

#ifndef CBUF
#define CBUF(X,L) (L+X)%L // assumes X >= -L
#endif

//typedef boost::multi_array <double, 2> CoefArray;
//typedef CoeffArray::index coef_array_index;

typedef std::vector <double> CoefArray;

//class Filt;

class Section{
 public:
  Section( CoefArray::iterator num_coef_first,    CoefArray::iterator num_coef_last,
	   CoefArray::iterator  denom_coef_first, CoefArray::iterator denom_coef_last,
	   double multiplier, bool forward_direction)
    : numerator_coefs   (num_coef_first, num_coef_last)
    , denominator_coefs (denom_coef_first, denom_coef_last)
    , multiplier        (multiplier)
    , forward_direction (forward_direction)
    {}
  void operator()();
  friend class Filt;
 private:
 
  CoefArray numerator_coefs, denominator_coefs;
  double multiplier;
  bool forward_direction;
  raw_voltage_circular_buffer *input_buffer;
  raw_voltage_circular_buffer *output_buffer;
};

class Filt{

 public:
  Filt();
  Filt( ArteFilterOptPb &filt_opt, int n_samps_per_source_chan );

  typedef std::map <std::string, Filt> FiltList;

  virtual ~Filt();

  void init(boost::property_tree::ptree &filt_pt);

  // do the filtering (new way)
  void operator()();

  raw_voltage_circular_buffer *attach_buffers( raw_voltage_circular_buffer *in_buffer );

  
  std::string filt_name;

  // Only for backwards compatibility.  filt_type isn't relevant in new way that filt class works
  name_string_t filt_type;               // iir or fir.  Determines how to interpret long lists of numerators and denominators
                                  // (iir case, they're treated as second-order sections.  fir case, one long kernel).
  double num_coefs [MAX_FILT_COEFS];                     // numerators.
  double denom_coefs [MAX_FILT_COEFS];                     // denominators. We don't calculate these.  I get them from matlab filter design tool.

  // Coef array rows are sections, columns are coefficients for that section
  // one SOS has three numerator coefs, three denominator coeffs


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

  CoefArray numerator_coefs;
  CoefArray denominator_coefs;
  std::vector < double > multipliers;  
  int delay_direction; // 1: filt  0: filtfilt  -1:all backwards
  bool make_sos;
  int filtfilt_invalid_samps;
  int my_minimum_samps;

  int n_sections;
  std::vector <Section> sections;

  void generate_coefs ( ArteFilterOptPb &filt_opt );

  //  static FiltList filt_list;
  static FiltList build_filt_list( ArteSetupOptPb & setup_opt, int n_samps_per_source_chan );

 private:
  raw_voltage_circular_buffer *input_buffer;
  raw_voltage_circular_buffer output_buffer;
  std::vector <raw_voltage_circular_buffer> intermediate_buffers;
  void init_raw_voltage_circular_buffer (raw_voltage_circular_buffer &buffer,
					 int n_chans, int n_samps);

};

int rel_pt(int pos, int curs, int buf_len);

void filter_data(rdata_t *in_buf, Filt *filt, neural_daq *nd,  uint16_t *chans, uint16_t n_chans, int in_buf_len, 
		  uint16_t out_buf_len, int *u_curs, int *f_curs, int *ff_curs, rdata_t* u_buf, rdata_t *f_buf, rdata_t *ff_buf);

void print_array(rdata_t *buf, int n_chans, int buf_len, int curs);
#endif
