// a_timer.cpp
// implement generic timer tasks

#include <iostream>
#include <string>
#include "a_timer.h"

aTimer::aTimer(){
  emitter_state = TIMER_UNINITIALIZED;
  counter_state = TIMER_UNINITIALIZED;
  id = -1;
}

aTimer::~aTimer(){
  // test state before destroying object
  if(emitter_state != TIMER_STOPPED | counter_state != TIMER_STOPPED){
    std::cerr << "aTimer destructor reached on non-stopped timer id:"
	      << id << std::endl;
  }
}



void aTimer::set_timer_id(int newID){
  id = newID;
}

int aTimer::get_timer_id(){
  return id;
}

double aTimer::get_arte_time_to_secs_coeff(){
  return arte_time_to_secs_coeff;
}

void aTimer::set_arte_time_to_secs_coeff(double new_coeff){
  arte_time_to_secs_coeff = new_coeff;
}

void aTimer::tic(){
  tic_time = this->get_count();
}

uint32_t aTimer::toc(){
  return (this->get_count() - tic_time);
}

double aTimer::toc_secs(){
  return (toc() * arte_time_to_secs_coeff);
}

void aTimer::print_state(){
  std::string eState, cState;
  if( emitter_state == TIMER_UNINITIALIZED )
    eState.assign("uninitialized");
  if( emitter_state == TIMER_STOPPED )
    eState.assign("stopped");
  if( emitter_state == TIMER_RUNNING )
    eState.assign("running");
  if( counter_state == TIMER_UNINITIALIZED )
    cState.assign("uninitialized");
  if(counter_state == TIMER_STOPPED )
    cState.assign("stopped");
  if(counter_state == TIMER_RUNNING )
    cState.assign("running");
  std::cout << "Emitter_state: " << eState << std::endl
	    << "Counter_state: " << cState << std::endl;
}
