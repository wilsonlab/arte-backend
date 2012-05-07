// system_clock_timer.h subclass of a_timer
// simulate a counter

#ifndef SYSTEM_CLOCK_TIMER_H_
#define SYSTEM_CLOCK_TIMER_H_

#include "a_timer.h"

class SystemClockTimer : public aTimer {
 public:
  SystemClockTimer();

  void start_counting();
  void stop_counting();
  void reset_count();
  void start_emitting();
  void stop_emitting();
  uint32_t get_count();
  double get_timestamp_secs();
  
  void init_as_clock_source();
  void init_as_counter();

  void print_state();

 protected:
  double completed_runs_time;
  double last_start_time;
  double get_absolute_time();

  void internal_start_counting();
  void internal_stop_counting();
};

#endif
