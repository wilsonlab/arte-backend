// ctr05_timer.h subclass of a_timer
// Use PCI-CTR05 card as a timer

#ifndef CTR05_TIMER_H_
#define CTR05_TIMER_H

#include "a_timer.h"
#include "pci-ctr05.h"

class Ctr05Timer : public aTimer{

 public:
  Ctr05Timer();

  void start_counting();
  void stop_counting();
  void reset_count();
  
  void start_emitting();
  void stop_emitting();
  uint32_t get_count();
  double get_timestamp_secs();

  void init_as_clock_source();
  void init_as_counter();
 
  void print_state();

 private:
  void DoOpenDevices();
  char *DevName = "/dev/ctr05/ctr0_01";
  int Mode = CTR05_COUNTER;
  int Status;
  int Print = 1;
  int fdDIOA;
  int fdctr_1;
  int fdctr_2;
};

#endif
