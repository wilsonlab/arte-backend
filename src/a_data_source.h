/** ADataSource - Interface to abstract data generator (daq card, DIO, camera?)
 *  
 * A data source should generate data periodically, and put it in
 * a known location.
 * The type of the data is a template parameter
 */

#ifndef A_DATA_SOURCE_H_
#define A_DATA_SOURCE_H_

#include <stdint.h>
#include <thread> // std::thread std::condition
#include <mutex>
#include <memory> // std::shared_ptr <>
#include <vector>
#include <stack>
#include <set>
#include <boost/multi_array.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/variant.hpp>
#include "global_state.h"
#include "global_defs.h"
#include "a_timer.h"

// helpful utility - where should it go?  Probably not in this file
// From post at wonderful GotW blog: http://www.gotw.ca/gotw/079.htm
template <class T>
struct PtrTo{
  typedef std::shared_ptr <T> Type;
  typedef std::vector < std::shared_ptr <T> > ListType;
};



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
//typedef int ListenerKey;

template <class DataType>
class ADataSource{


 public:

  typedef typename PtrTo<ADataSource <DataType> >::Type SourcePtr;
  typedef typename PtrTo<ADataSource <DataType> >::ListType SourceList;
  // start should produce data, and only be called after
  // all worker threads have been spawned and begun waiting
  // for data
  virtual void start();
  virtual void stop();

  static std::shared_ptr <aTimer> timer_p;

  void register_listener( bool *smudge_ptr, 
			  std::condition_variable cond_var );
  void register_listener( bool *smudge_ptr );
  void unregister_listener( bool *smudge_ptr,
			    std::condition_variable cond_var );
  void unregister_listener( bool *smudge_ptr );

  std::set <bool *> smudge_set;
  std::set <std::condition_variable> data_ready_cond_var_list;

  void print_smudge_set();
  //std::stack <ListenerKey> listeners;
  //std::stack <ListenerKey> dirty_list;


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

  ArteGlobalState *global_state_p;


 private:

};


template <class DataType>
DataType & ADataSource<DataType>::get_data(){ 
  return data;
}


template <class DataType>
void ADataSource<DataType>::set_data(DataType &new_data){
  
  {
    std::unique_lock <std::mutex> data_lk (global_state_p->data_mutex);
    //std::cout << "Source got mutex\n";

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
    system("clear");
    std::cout << "SOURCE About to update data\n";
    for (auto it = smudge_set.begin(); it != smudge_set.end(); it++){
      //assert( ! ( **it) ); // assert that this listener has already processed
      (**it) = true; // Mark the listener's data as dirty (needs processing)
    }

    print_smudge_set();
    std::cout << "SOURCE about to notify_all\n";
    global_state_p->data_ready_cond.notify_all();
  }
}


template <class DataType>
void ADataSource<DataType>::start(){
  std::cout << "start() called on abstract data source.\n";
}


template <class DataType>
void ADataSource<DataType>::stop(){

  std::unique_lock <std::mutex> lk (global_state_p->data_mutex);
  data_is_valid = false;
  global_state_p->data_ready_cond.notify_all();

}

template <class DataType>
void ADataSource<DataType>::register_listener ( bool *smudge_ptr,
						std::condition_variable cond_var ){

  std::unique_lock <std::mutex> lk (global_state_p->data_mutex);
  smudge_set.insert( smudge_ptr );
  data_ready_cond_var_list.insert( cond_var );
  print_smudge_set();

}

template <class DataType>
void ADataSource<DataType>::register_listener ( bool *smudge_ptr ){
  std::unique_lock <std::mutex> lk (global_state_p->data_mutex);
  smudge_set.insert( smudge_ptr );
  print_smudge_set();
}

template <class DataType>
void ADataSource<DataType>::unregister_listener( bool *smudge_ptr,
						 std::condition_variable cond_var ){

  std::unique_lock <std::mutex> lk (global_state_p->data_mutex);
  smudge_set.erase( smudge_ptr );
  data_ready_cond_var_list.erase( cond_var );

}

template <class DataType>
void ADataSource<DataType>::unregister_listener( bool *smudge_ptr ){
  std::unique_lock <std::mutex> lk (global_state_p->data_mutex);
  smudge_set.erase( smudge_ptr );
}

template <class DataType>
void ADataSource<DataType>::print_smudge_set(){
  std::string list_string ("SOURCE: Smudge_set: ");
  char buff[20];

  for(auto it = smudge_set.begin(); it != smudge_set.end(); it++){
    sprintf(buff, "%x |", *it);
    list_string += buff;
  }
      //  std::cout << *it << " | ";
  list_string += '\n';
  std::cout << list_string;
}
      


template <class DataType>
std::shared_ptr <aTimer> ADataSource <DataType>::timer_p;

#endif
