#ifndef TRODE_H_
#define TRODE_H_

#include <NIDAQmx.h>
#include "arteopt.h"  

enum trig_mode_t {CENTER_ON_THRESH=0, CENTER_ON_NEXT_MAX=1};

Class Trode{

 public:
  Trode::Trode();
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

 private:

  TrodeDrawer td;

  ArteOpt opt;

  float64 *buffer;
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
