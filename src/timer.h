#include <iostream>
#include <sys/time.h>
//include <counter_card_driver.h>

enum timer_role_t {TIMER_ROLE_MASTER = 0, TIMER_ROLE_SLAVE = 1}; // a timer_role type 
enum timer_method{TIMER_METHOD_SYSCLOCK = 0, TIMER_METHOD_COUNTERCARD = 1}; // the type of timer

class Timer {
  
 private:
  
  timer_role_t timer_role;  // is this master timer (TIMER_ROLE_MASTER) or slave (TIMER_ROLE_SLAVE)
  unsigned long int raw_count_value; // the count value on the countercard, if we read that
  unsigned long int count_offset_value; // we may want to store a value 
  bool timer_armed; // armed = able to start running at the next sync-start pulse
  bool timer_running; // running = incrementing at each tic pulse
  unsigned long int hotstart_count_value; // after power-cycling a slave, this is part of the re-sync protocol
  timeval sys_time_at_counter_start;
  
 public:
  
  Timer();
  virtual ~Timer();
  int timer_init();
  unsigned long int get_count();

  void set_count(unsigned long int); // an override function.  Generally shouldn't be used.

  int write_count_to_es();
  int write_count_to_es_with_message(char *msg);

  int handle_sync_prepare_pulse();
  int handle_sync_start_pulse();
  int handle_tic_pulse();

  // make a packet giving the count value at current sys time.  Send it to
  // another machine to check if its count value was the same at its systime
  unsigned long int test_sysclock_sync(int); 

 private:
  unsigned long int get_count_by_sysclock();
  unsigned long int get_count_by_countercard();

}
