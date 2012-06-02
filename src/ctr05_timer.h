// ctr05_timer.h subclass of a_timer
// Use PCI-CTR05 card as a timer

#ifndef CTR05_TIMER_H_
#define CTR05_TIMER_H

#include <thread>
#include <mutex>
#include <string>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "a_timer.h"
#include "pci-ctr05.h"

//typedef void (*CALLBACK_FN)(void *);

class Ctr05Timer : public aTimer{

 public:
  Ctr05Timer(std::string &timer_mode);
  ~Ctr05Timer();

  void start_counting();
  void stop_counting();
  void reset_count();
  
  void start_emitting();
  void stop_emitting();
  uint32_t get_count();
  double get_timestamp_secs();

 
  void print_state();

  bool is_counting();

 private:

  //char DevName[30];
  int Mode;
  //int Status;
  //int Print;
  int fdDIOA;
  int fdctr_1;
  int fdctr_2;
  int freq;
  // TODO replace uint32_t with timestamp_t
  uint32_t total_count, total_count_max;
  unsigned short small_counter_max, this_small_counter_val;
  
  void init_as_clock_source();
  void init_as_counter();
 
  void touch_count();
  
};
/*
struct CountTouch{
  CountTouch( Ctr05Timer *timer );
  Ctr05Timer *my_timer;
  void operator()();
}
*/
#endif
