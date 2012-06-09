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
#include <stack>
//#include <boost/thread/shared_mutex.hpp>
//#include <boost/thread/condition_variable.hpp>
#include <boost/multi_array.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/variant.hpp>
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
typedef int ListenerKey;

template <class DataType>
class ADataSource{

 public:

  // start should produce data, and only be called after
  // all worker threads have been spawned and begun waiting
  // for data
  virtual void start() = 0;
  virtual void stop()  = 0;

  static std::shared_ptr <aTimer> timer_p;

  void register_listener( ListenerKey );
  
  std::stack <ListenerKey> listeners;
  std::stack <ListenerKey> dirty_list;


  DataType data;
  DataType scratch_data;
  bool data_is_valid;
  // Note: timestamp should be a property of DataType itself,
  // I don't think we should use this extra member for timestamp
  timestamp_t data_timestamp;


  void set_data( DataType &new_data );
  DataType &  get_data();
  uint64_t update_count;

  data_source_state_t state;

  std::shared_ptr <ArteGlobalState> global_state_p;

  //std::mutex                registry_mutex;
  std::mutex                    data_mutex;
  std::condition_variable  data_ready_cond;

 private:

};


template <class DataType>
DataType & ADataSource<DataType>::get_data(){ 
  return data;
}


template <class DataType>
void ADataSource<DataType>::set_data(DataType &new_data){
  
  {
    //   std::unique_lock <std::mutex> reg_lk  (registry_mutex);
    std::unique_lock <std::mutex> data_lk (data_mutex);

    // right now we update the data using DataType copy constructor
    // TODO: make virtual function for other methods of touching the data
    // (e.g. - probably faster to have a double-buffer kind of strategy,
    //         where setting the data just means swapping the pointers)
    // default a_data_source can do it as double-buffer, subclasses may override
    data = new_data;
    data_is_valid = true;
    update_count++;
    
    // Assert that all listeners have finished copying the prior data
    // Mark all listeners as dirty by adding their labels to dirty-list
    assert( dirty_list.empty() );
    dirty_list (listeners);
    
    data_ready_cond.notify_all();
  }
}


template <class DataType>
void ADataSource<DataType>::stop(){

  std::unique_lock <std::mutex> lk (data_mutex);
  data_is_valid = false;
  data_ready_cond.notify_all();

}

template <class DataType>
void ADataSource<DataType>::register_listener (ListenerKey the_key){
  std::unique_lock <std::mutex> lk (data_mutex);
  listeners.push( the_key );
}

template <class DataType>
std::shared_ptr <aTimer> ADataSource <DataType>::timer_p;

typedef std::shared_ptr < ADataSource <NeuralVoltageBuffer> > NeuralVoltageSourcePtr;

class NidaqDataSource;
typedef std::shared_ptr <NidaqDataSource> NidaqDataSourcePtr;
class MockDataSource;
typedef std::shared_ptr <MockDataSource> MockDataSourcePtr;

typedef boost::variant < NidaqDataSourcePtr, MockDataSourcePtr > AnyDataSource;
typedef std::vector <AnyDataSource> data_source_list;

#endif
