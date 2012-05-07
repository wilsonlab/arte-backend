#include <sys/time.h>
#include <iostream>
#include "system_clock_timer.h"


SystemClockTimer::SystemClockTimer(){
  emitter_state = TIMER_UNINITIALIZED;
  counter_state = TIMER_UNINITIALIZED;
  init_as_clock_source();
  init_as_counter();
  set_arte_time_to_secs_coeff( 1.0 / 10000.0 );
}

double SystemClockTimer::get_absolute_time(){
  timeval tim;
  gettimeofday(&tim, NULL);
  return (double) tim.tv_sec + (double)tim.tv_usec/1000000.0;
}

void SystemClockTimer::init_as_clock_source(){
  emitter_state = TIMER_STOPPED;
}

void SystemClockTimer::init_as_counter(){
  counter_state = TIMER_STOPPED;
  completed_runs_time = 0.0;
  last_start_time = 0.0;
}

void SystemClockTimer::start_counting(){
  // only need to do something if emitter is running and counting gets toggled
  // we 'count' when counter is counting AND emitter is emitting
  if( emitter_state == TIMER_RUNNING & counter_state == TIMER_STOPPED)
    internal_start_counting();
  counter_state = TIMER_RUNNING;
}

void SystemClockTimer::stop_counting(){
  // only need to do something if we're currently counting
  if( emitter_state == TIMER_RUNNING & counter_state == TIMER_RUNNING)
    internal_stop_counting();
  counter_state = TIMER_STOPPED;
}

void SystemClockTimer::start_emitting(){
  // only do something if counting and we toggle emitting
  if( emitter_state == TIMER_RUNNING & counter_state == TIMER_STOPPED )
    internal_start_counting();
  emitter_state = TIMER_RUNNING;
}

void SystemClockTimer::stop_emitting(){
  // only do something if we are already counting & emitting
  if( emitter_state == TIMER_RUNNING & counter_state == TIMER_RUNNING )
    internal_stop_counting();
  emitter_state = TIMER_STOPPED;
}

void SystemClockTimer::internal_start_counting(){
  last_start_time = get_absolute_time();
}

void SystemClockTimer::internal_stop_counting(){
  completed_runs_time += (get_absolute_time() - last_start_time);
}

double SystemClockTimer::get_timestamp_secs(){
 if( emitter_state == TIMER_RUNNING & counter_state == TIMER_RUNNING )
   return completed_runs_time + (get_absolute_time() - last_start_time);
 else
   return completed_runs_time;
}
  
uint32_t SystemClockTimer::get_count(){
  return (uint32_t)(get_timestamp_secs() / arte_time_to_secs_coeff);
}

void SystemClockTimer::reset_count(){
  last_start_time = get_absolute_time();
  completed_runs_time = 0.0;
}

void SystemClockTimer::print_state(){
  std::cout << "last_start_time: " << last_start_time << std::endl
	    << "completed_runs_time: " << completed_runs_time << std::endl;
  aTimer::print_state();
}
