// a_timer.h  
// Defines abstract base class for timers
//
// A timer instance in arte can be a clock source or a counter
// Expected derived classes:
// -systemTimer   counter only, gets time from system time and converts it to arte time units
// -manualTimer   counter only, gets 'time' from owner calling 'increment'.
// -NIDAQmxTimer  counter or timer, using M-series multifunction National Instruments card
// -PCI05Timer    counter or timer, using PCI-05 coutner card

class aTimer {

 public:
  aTimer();                                 // constructor
  virtual void init_as_clock_source() = 0;  // 
  virtual void init_as_counter() = 0;
  start();
  stop();
  int get_timer_id();
  set_timer_id(int newID);
  uint32_t get_count();
  uint32_t get_timestamp();
  double get_arte_time_to_secs_coeff();
  void   set_arte_time_to_secs_coeff(double new_coeff);
  increment();
  reset_count();
  int tic();
  uint32_t toc();
  
 private:
  uint32_t tic_time;
  double arte_time_to_secs_coeff;
};
