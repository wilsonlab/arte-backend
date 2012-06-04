/** ADataSource - Interface to abstract data generator (daq card, DIO, camera?)
 *  
 * A data source should generate data periodically, and put it in
 * a known location.
 * The type of the data is a template parameter
 */

#ifndef A_DATA_SOURCE_H_
#define A_DATA_SOURCE_H_

#include <stdint.h>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include "global_state.h"
#include "a_timer.h"

enum data_source_state_t = {DATA_SOURCE_INVALID, 
			    DATA_SOURCE_STOPPED, 
			    DATA_SOURCE_RUNNING};

template <class DataType>
class ADataSource{

 public:

  ADataSource( boost::shared_ptr <ATimer> a_timer );

  // start should produce data, and only be called after
  // all worker threads have been spawned and begun waiting
  // for data
  virtual void start() = {};
  virtual void stop()  = {};

  DataType & get_data();
  timestamp_t get_data_timestamp();


 private:

  void set_data( DataType &new_data );

  DataType data;
  bool data_is_valid;
  timestamp_t data_timestamp;
  uint64_t update_count;

  data_source_state_t state;

  std::shared_ptr <ArteState> global_state_p;
  std::shared_ptr <ATimer> timer_p;

  boost::shared_mutex data_mutex;
  boost::condition_variable data_ready_cond;
  
};

// defined here rather than .cpp because relies on template param
// Many reader threads may request data.  They each share a mutex
// and wait for the data_ready condition variable
void ADataSource::get_data(DataType & return_data){
  
  boost::mutex::shared_lock lk ( data_mutex );
  data_ready_cond.wait( lk );
  // condition variable indicates data is ready
  // take the shared_lock again while copying the member
  // data back to the caller
  boost::mutex::shared_lock lk ( data_mutex );
  return_data = data;
  lk.unlock_shared();

}

// a single writer thread (the one owning this instance of ADataSource
// competes with all readers for the data_lock, notifies them all
// after new_data is copied into the data member
void ADataSource::set_data(DataType &new_data){
  
  boost::mutex::scoped_lock ( data_mutex );
  data = new_data;
  data_is_valid = true;
  update_count++;
  data_ready_cond.notify_all();

}

#endif
