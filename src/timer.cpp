#include "timer.h"

Timer::Timer(){
  tics_per_sec = 100000.0;
  secs_per_tic = 1.0 / tics_per_sec;
}

Timer::~Timer(){

}

int Timer::timer_init(){
  timer_armed = true;
  timer_running = false; // wait for a start command?
}

