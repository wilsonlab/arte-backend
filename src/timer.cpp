#include "timer.h"

const int tics_per_sec = 100000; 
const double secs_per_tic = 1.0 / (double)tics_per_sec;

Timer::Timer(){

}

Timer::~Timer(){

}

Timer::timer_init(){
  timer_armed = true;
  timer_running = false; // wait for a start command?
}

