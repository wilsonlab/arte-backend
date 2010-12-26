#ifndef TRODE_H_
#define TRODE_H_

#include <NIDAQmx.h>

enum trig_mode_t {CENTER_ON_THRESH=0, CENTER_ON_NEXT_MAX=1};

// not a method, but a regular function
// this way has less overhead.
// fn gets pointer to trode, so it has access
// to all the public member variables
void trode_filter_data(Trode *, float64 *, int);  // trode_filter_data(trode_to_process, raw buffer, timestamp at buffer start)
void trode_process_data(Trode *); // only need access to this trode's buffers (filtered data is there)

Class Trode{

 public:
  // Constructor will be called by arteopt.cpp itself, to avoid this class having
  // an include for arteopt.h (prefer arte opt includes trode, for scope reasons;
  // arteopt now does all the initialization stuff, and needs a std::vector<Trode>
  Trode::Trode();
  Trode::Trode(std::string name, int n_chans, float64 *thresholds, int samps_pre, int samps_post, int trig_mode,
	       std::string filt_name);

  // The old idea: constructor just takes a tetrode name and the options structure. 
  Trode::Trode(char *, opt *)   // override constructor, take just a name and opt object
                                // then let the trode finish initializing

  virtual Trode::~Trode();

  void set_filter();
  void set_trig_params();
  void set_n_chans;
  void set_n_samps_per_chan();
  int get_n_chans;
  int get_n_samps_per_chan();
  
  void remove_all_drawing_areas();
  void add_drawing_area();

  //add_data(prt_to_array, n_chans, n_samps_per_chan, time_at_beginning_of_buffer)
  void add_data(float64 *,int,int,long int);
  // NB: we'll prob use the fn's trode_filter_data & trode_process_data instead of this method
  // to save overhead.  But let's keep this member fn and implement it so we can compare the execution time

  float64 *raw_buffer;
  float64 *filtered_buffer;
  int data_cursor;
  int cursor_time;

  int n_chans;                // num of chans for this trode
  int n_samps_per_chan;       // num of samps per chan
  int *chan_inds;             // ptr is to 32-chan signal.  Which rows belong to this trode?

  trig_mode_t trig_mode;      // instruction on how to center spike events, on trigger crossing or next local max?
  float64 *thresholds;        // array of threshold voltages, 1 per chan
  int samps_before_trig;      // how many samps to collect before trig ind
  int samps_after_trig;       // hom many samps to collect after trig ind (total samps is after + before + 1


#endif
