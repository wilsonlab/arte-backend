#include <iostream>
#include <sys/ioctl.h>
#include "ctr05_timer.h"


Ctr05Timer::Ctr05Timer(){
  set_arte_time_to_secs_coeff( 1.0 / 10000.0 );
}


void Ctr05Timer::init_as_clock_source(){
  char str[] = "/dev/ctr05/ctr0_02";
  if ((fdctr_2 = open(str, CTR05_COUNTER)) < 0){
    perror(str);
    printf("ctr05_timer error:\n");
    printf("error opening device %s",str);
  } else{
    emitter_state = TIMER_STOPPED;
  }
}


void Ctr05Timer::init_as_counter(){
  char str[] = "/dev/ctr05/ctr0_01";
  if ((fdctr_1 = open(str, CTR05_COUNTER)) < 0){
    perror(str);
    printf("ctr05_timer error:\n");
    printf("error opening device %s",str);
  } else {
    counter_state = TIMER_STOPPED;
    total_count = 0;
    this_counter_val = 0;
    previous_counter_val = 0;
    big_internal_count = 0;
    uint16_max = 0xFFFF;
    pthread_mutex_init( &get_count_mutex, NULL );
    int rc = pthread_create( &poll_thread, NULL, touch_count, (void*)this );
  }
}

void * Ctr05Timer::touch_count( void* data ){
  while (this->is_counting()){
    printf("in touch_count()\n");
    data->get_count();
    sleep(1);
  }
}

void Ctr05Timer::start_counting(){
  if( counter_state == TIMER_STOPPED ){
    last_start = get_count();
  }
  counter_state = TIMER_RUNNING;
}

void Ctr05Timer::stop_counting(){
  if( counter_state == TIMER_RUNNING ){
    accumulated_finished_blocks += (get_count() - last_start);
  }
  counter_state == TIMER_STOPPED;
}

uint32_t Ctr05Timer::get_count(){
  pthread_mutex_lock( &get_count_mutex );
  previous_small_counter_val = this_small_counter_val;
  read(fdctr_1, &this_small_counter_val, 1);
  if (this_small_counter_val < previous_small_counter_val){
    printf("Added to big_internal_count\n");
    big_internal_count += small_counter_max;
  }
  if ( (total_count + this_small_counter_val) > total_count_max){
    std::err << "Ctr05Timer Error: Counter uint32_t overflow, " 
	     << "please restart system or reset all counters.\n" 
	     << std::endl;
  }
  return (big_internal_count + this_small_counter_val);
  pthread_mutex_unlock( &get_count_mutex );
}

bool Ctr05::Timer::is_counting(){
  return counter_state == TIMER_RUNNING;
}

void Ctr05Timer::start_emitting(){
  if( emitter_state == TIMER_STOPPED ){
    ioctl(fdctr_2, SET_SQUARE_FREQ, int(1.0/ arte_time_to_secs_coeff));
  }
  if ( emitter_state == TIMER_UNINITIALIZED ){
    std::cerr << "Ctr05Timer error: tried to start emitting when uninitialized"
	      << std::endl;
  }
}
    
void DoOpenDevices()
{
  char str[80];

  strcpy(str, "/dev/ctr05/dio0_0A");
  if ((fdDIOA = open(str, Mode)) <0) {
    perror(str);
    printf("ctr05_timer error:\n");
    printf("error opening device %s\n", str);
    exit(2);
  }

  // input counter
  strcpy(str, "/dev/ctr05/ctr0_01");
  if ((fdctr_1 = open(str, CTR05_COUNTER)) < 0) {
    perror(str);
    printf("ctr05_timer error:\n");
    printf("error opening device %s\n",str);
    exit(2);
  }

  // 
  strcpy(str, "/dev/ctr05/ctr0_02");
  if ((fdctr_2 = open(str, CTR05_COUNTER)) < 0) {
    perror(str);
    printf("ctr05_timer_error:\n");
    printf("error opening device %s",str);
  }

  // frequency generator
  strcpy(str, "/dev/ctr05/ctr0_03");
  if ((fdctr_3 = open(str, CTR05_FREQUENCY)) < 0) {
    perror(str);
    perror(str);
    printf("error opening device %s\n", str);
  }

}
