// a_timer.h  
// Defines abstract base class for timers
//
// A timer instance in arte can be a clock source or a counter
// Expected derived classes:
// -systemTimer   counter only, gets time from system time and converts it to arte time units
// -manualTimer   counter only, gets 'time' from owner calling 'increment'.
// -NIDAQmxTimer  counter or timer, using M-series multifunction National Instruments card
// -PCI05Timer    counter or timer, using PCI-05 coutner card

// clock source needs to be able to:
//  - start/stop generating pulses
//  - set/get pulse rate 
//  - log system time when timer is started (maybe also at regular intervals)
//
// counter needs to be able to:
//  - start/stop counting pulses (if hardware counter)
//  - reset the count
//  - have an offset? 
//  - respond to requests for the current count
//  - cache last count?
//  - report count in raw units, or in seconds
//  - know the pulse rate (real if hardware counter, imagined if sys clock)

#ifndef A_TIMER_H_
#define A_TIMER_H_

#include "arte_command.pb.h"

enum timer_state_t {TIMER_UNINITIALIZED, TIMER_STOPPED, TIMER_ARMED, TIMER_RUNNING};


class aTimer {

 public:
  aTimer();                                 // constructor
  ~aTimer();
  virtual void init_as_clock_source() = 0;  // 
  virtual void init_as_counter() = 0;

  virtual void start_counting() = 0;
  virtual void stop_counting() = 0;
  virtual void reset_count() = 0;
  virtual void start_emitting() = 0;
  virtual void stop_emitting() = 0; 

  int get_timer_id();
  void set_timer_id(int newID);

  virtual uint32_t get_count() = 0;
  virtual double get_timestamp_secs() = 0;

  void print_state();

  double get_arte_time_to_secs_coeff();
  void   set_arte_time_to_secs_coeff(double new_coeff);

  void tic();
  uint32_t toc();
  double   toc_secs();

  int process_command(ArteCommand& the_command);
  
 protected:
  int id;
  bool is_clock_source, is_counter;
  timer_state_t emitter_state;
  timer_state_t counter_state;
  uint32_t last_retrieved_count;
  uint32_t tic_time;
  double arte_time_to_secs_coeff;

};

#endif
