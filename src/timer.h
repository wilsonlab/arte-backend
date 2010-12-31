#include <iostream>
#include <sys/time.h>


//enum timer_role_t {TIMER_ROLE_MASTER = 0, TIMER_ROLE_SLAVE = 1}; // a timer_role type 
//enum timer_method{TIMER_METHOD_SYSCLOCK = 0, TIMER_METHOD_COUNTERCARD = 1}; // the type of timer

class Timer {
  
 public:
  
  Timer();
  virtual ~Timer();
  int timer_init();

  unsigned long get_count();

  void set_count(unsigned long); // an override function.  Generally shouldn't be used.

  int write_count_to_es();
  int write_count_to_es_with_message(char *msg);

  int handle_sync_prepare_pulse();
  int handle_sync_start_pulse();
  int handle_tic_pulse();

  // make a packet giving the count value at current sys time.  Send it to
  // another machine to check if its count value was the same at its systime
  unsigned long test_sysclock_sync(int); 

 private:
  //timer_role_t timer_role; // is this master timer or slave?
  
  int timer_role;
  unsigned long raw_count_value; // count value on the countercard, if we read that
  unsigned long count_offset_value; // we may want to store an offset
  bool timer_armed; // armed = able to start running at the next sync-start pulse?
  bool timer_running; // running = incrementing at each tic pulse
  unsigned long hotstart_count_value; // after power-cycling as lave, this is part of the re-sync protocol
  timeval sys_time_at_counter_start;

  double tics_per_sec;
  double secs_per_tic;

  unsigned long get_count_by_sysclock();
  unsigned long get_count_by_countercard();

};
