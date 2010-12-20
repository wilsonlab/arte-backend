#include <NIDAQmx.h>

class Filt{

 public:
  Filt();
  virtual ~Filt();

  void init();

 private:
  int n_chans;
  int n_samps_per_chan;
  int data_cursor;

  float64 *x_buffer;
  float64 *y_buffer;

  bool filtfilt;

  int filt_order;
  float64 *a; //denominator filter coefficients
  float64 *b; //numerator filter coefficients
  int filt_num_sos;  // how many biquad sections (should 1 be if FIR)



