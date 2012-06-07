/** ADataSource - Interface to abstract data generator (daq card, DIO, camera?)
 *  
 * A data source should generate data periodically, and put it in
 * a known location.
 * The type of the data is a template parameter
 */

#ifndef A_DATA_SOURCE_H_
#define A_DATA_SOURCE_H_

#include <stdint.h>
#include <thread> // std::thread std::mutex std::condition  TODO: clean this list up
#include <mutex>
#include <memory> // std::shared_ptr <>
//#include <boost/thread/shared_mutex.hpp>
//#include <boost/thread/condition_variable.hpp>
#include <boost/multi_array.hpp>
#include <boost/circular_buffer.hpp>
#include "global_state.h"
#include "global_defs.h"
#include "a_timer.h"

// define some types of data
typedef boost::multi_array <rdata_t,2> raw_voltage_array;
typedef std::vector < boost::circular_buffer <rdata_t> > raw_voltage_circular_buffer;
typedef raw_voltage_array::index voltage_array_index;

struct NeuralVoltageBuffer{
  timestamp_t       one_past_end_timestamp;
  raw_voltage_array voltage_buffer;
};

struct NeuralVoltageCircBuffer{
  timestamp_t                 one_past_end_timestamp;
  raw_voltage_circular_buffer voltage_buffer;
};

enum data_source_state_t  {DATA_SOURCE_INVALID, 
			   DATA_SOURCE_STOPPED, 
			   DATA_SOURCE_RUNNING};

template <class DataType>
class ADataSource{

 public:

  // start should produce data, and only be called after
  // all worker threads have been spawned and begun waiting
  // for data
  virtual void start() = 0;
  virtual void stop()  = 0;

  DataType & get_data();
  timestamp_t get_data_timestamp();
  static std::shared_ptr <aTimer> timer_p;

  DataType data;
  DataType scratch_data;
  bool data_is_valid;
  timestamp_t data_timestamp;


  void set_data( DataType &new_data );
  void get_data( DataType &data );
  uint64_t update_count;

  data_source_state_t state;

  std::shared_ptr <ArteGlobalState> global_state_p;

  std::mutex data_mutex;
  std::condition_variable data_ready_cond;

 private:

};


// defined here rather than .cpp because relies on template param
// Many reader threads may request data.  They each share a mutex
// and wait for the data_ready condition variable
template <class DataType>
void ADataSource<DataType>::get_data(DataType & return_data){
  
  std::unique_lock<std::mutex> lk ( data_mutex );
  data_ready_cond.wait( lk );
  // condition variable indicates data is ready
  // take the shared_lock again while copying the member
  // data back to the caller
  { 
    std::lock_guard<std::mutex> lk ( data_mutex );
    return_data = data;
  }
}

// a single writer thread (the one owning this instance of ADataSource
// competes with all readers for the data_lock, notifies them all
// after new_data is copied into the data member
template <class DataType>
void ADataSource<DataType>::set_data(DataType &new_data){
  
  boost::mutex::scoped_lock ( data_mutex );
  data = new_data;
  data_is_valid = true;
  update_count++;
  data_ready_cond.notify_all();

}


template <class DataType>
void ADataSource<DataType>::stop(){

  std::lock_guard <std::mutex> lk (data_mutex);
  data_is_valid = false;
  data_ready_cond.notify_all();

}

template <class DataType>
timestamp_t ADataSource<DataType>::get_data_timestamp(){
  return timer_p->get_count();
}

template <class DataType>
std::shared_ptr <aTimer> ADataSource <DataType>::timer_p;


#endif
