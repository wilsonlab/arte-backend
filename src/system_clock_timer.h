// system_clock_timer.h subclass of a_timer
// simulate a counter

#ifndef SYSTEM_CLOCK_TIMER_H_
#define SYSTEM_CLOCK_TIMER_H_

#include "a_timer.h"

class SystemClockTimer : public aTimer(){
 public:
  SystemClockTimer();
  ~SystemClockTimer();
  void start_counting();
  void stop_counting();
  void reset_count();
  void start_emitting();
  void stop_emitting();
 private:
  double completed_runs_time;
  double last_start_time;
};
