// a_timer.cpp
// implement generic timer tasks

#include <iostream>

aTimer::aTimer(){
  timer_state = uninitialized;
  id = -1;
}

aTimer::~aTimer(){
  // test state before destroying object
  if(timer_state != TIMER_STOPPED){
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


