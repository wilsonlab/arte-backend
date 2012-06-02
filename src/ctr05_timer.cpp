#include <stdio.h>
#include <algorithm>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "ctr05_timer.h"
#include "pci-ctr05.h"

#define CTR2_BIT    0x2

Ctr05Timer::Ctr05Timer(std::string &timer_mode){
  freq = 10000;
  set_arte_time_to_secs_coeff( 1.0 / freq );
  //strcpy( &DevName[0], "/dev/ctr05/ctr0_01" );
  Mode = CTR05_COUNTER;
  //Print = 1;
  if( !(timer_mode.find("e") == std::string::npos) )
    init_as_clock_source();
  if( !(timer_mode.find("c") == std::string::npos) )
    init_as_counter();
  total_count_max = UINT32_MAX;
}


void Ctr05Timer::init_as_clock_source(){
  char str[] = "/dev/ctr05/ctr0_02";
  if ((fdctr_2 = open(str, Mode)) < 0){
    perror(str);
    printf("ctr05_timer error:\n");
    printf("error opening device %s",str);
  } else{
    emitter_state = TIMER_STOPPED;
  }
}


void Ctr05Timer::init_as_counter(){
  char str[] = "/dev/ctr05/ctr0_01";
  if ((fdctr_1 = open(str, Mode)) < 0){
    perror(str);
    printf("ctr05_timer error:\n");
    printf("error opening device %s",str);
  } else {
    unsigned short source = SRC1;
    write(fdctr_1, &source, 2);
    counter_state = TIMER_STOPPED;
    total_count = 0;
    this_small_counter_val = 0;
  }
}

void Ctr05Timer::touch_count(){
  while ( is_counting() ){
    //printf("in the thread\n");
    get_count();
    sleep(1);
  }
}

void Ctr05Timer::start_counting(){
  if( counter_state == TIMER_STOPPED ){
    counter_state = TIMER_RUNNING;
    std::cout << " about to start thread" << std::endl; fflush(stdout);
    std::thread t(&Ctr05Timer::touch_count, this);
    //touch_thread.swap(t);
    //t.destroy();
    //std::thread t2( std::move(t) );
    //touch_thread.detach();
    //touch_thread(&Ctr05Timer::touch_count, this);
    t.detach();
    std::cout << "finished starting thread" << std::endl; fflush(stdout);
  }
}

void Ctr05Timer::stop_counting(){
  if( counter_state == TIMER_RUNNING ){
    // touch the count once before stopping, to bring current count into the total
    get_count();
  }
  counter_state == TIMER_STOPPED;
}

uint32_t Ctr05Timer::get_count(){

  if (counter_state == TIMER_RUNNING){
    std::lock_guard <std::mutex> lk ( get_count_mutex );
    
    read(fdctr_1, &this_small_counter_val, 1);
    unsigned short source = SRC1;
    write(fdctr_1, &source, 2);
    
    total_count += this_small_counter_val;
    
    if ( (total_count + this_small_counter_val) > total_count_max){
      std::cerr << "Ctr05Timer Error: Counter uint32_t overflow, " 
		<< "please restart system or reset all counters.\n" 
		<< std::endl
		<< "(total) = " << (total_count)
		<< "  total_count_max = " << total_count_max << std::endl
		<< "   small = " << this_small_counter_val << std::endl;
    }
  }
  return total_count;
}

bool Ctr05Timer::is_counting(){
  return counter_state == TIMER_RUNNING;
}

void Ctr05Timer::start_emitting(){
  if( emitter_state == TIMER_STOPPED ){
    ioctl(fdctr_2, SET_SQUARE_FREQ, freq);
    emitter_state = TIMER_RUNNING;
  }
  if ( emitter_state == TIMER_UNINITIALIZED ){
    std::cerr << "Ctr05Timer error: tried to start emitting when uninitialized"
	      << std::endl;
  }
}
    
void Ctr05Timer::stop_emitting(){
  if( emitter_state == TIMER_RUNNING ){
    ioctl(fdctr_2, LOAD_CMD_REG, DISARM | CTR2_BIT);
    emitter_state = TIMER_STOPPED;
  }
}

double Ctr05Timer::get_timestamp_secs(){
  return (get_count() * arte_time_to_secs_coeff);
}

void Ctr05Timer::reset_count(){
  // reset the hardware counter
  unsigned short source = SRC1;
  write(fdctr_1, &source, 2);
  // and reset the cumulative count
  total_count = 0;
}

void Ctr05Timer::print_state(){
  std::string emitter_string, counter_string;
  (counter_state == TIMER_RUNNING) ? counter_string.assign("counter counting")
    : counter_string.assign("counter not counting");
  (emitter_state == TIMER_RUNNING) ? emitter_string.assign("emitter emitting")
    : emitter_string.assign("emitter not emitting");
  std::cout << emitter_string << std::endl << counter_string << std::endl
	    << "is_couning(): " << is_counting() << std::endl;
}

Ctr05Timer::~Ctr05Timer(){
  
  printf("about to close files\n");
  close(fdctr_1);
  close(fdctr_2);
}

/*
CountTouch::CountTouch( Ctr05Timer *timer )
  : my_timer( timer );
{ }

CountTouch::operator()(){
  while ( timer->is_counting() ){
    timer->get_count();
    sleep(1);
  }
}
*/
