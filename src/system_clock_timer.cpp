#include <sys/time.h>
#include "system_clock_timer.h"

SystemClockTimer::SystemClockTimer(){
  emitter_state = TIMER_UNINITIALIZED;
  counter_state = TIMER_UNINITIALIZED;
  init_as_clock_source();
  init_as_counter();
  tic();
}

SystemClockTimer::~SystemClockTimer(){
  aTimer::~aTimer();
}

void SystemClockTimer::initialize_as_clock_source(){
  emitter_state = TIMER_STOPPED;
}

void SystemClockTimer::initialize_as_counter(){
  counter_state = TIMER_STOPPED;
}

void SystemClockTimer::start_counting(){

  // already counting but not emitting -> do nothing
  // neither counting nor emitting -> do nothing
  if( emitter_state = TIMER_STOPPED )
    {};
  // emitter is emitting, counter already counting -> nothing
  elseif( emitter_state = TIMER_RUNNING & 
      counter_state = TIMER_RUNNING)
    {};
  // if emitter is emitting and we transition to counting
  else{
    last_start_time = toc_secs();
  }
  counter_state = TIMER_RUNNING;
}
