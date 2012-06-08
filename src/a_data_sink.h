#ifndef A_DATA_SINK_H_
#define A_DATA_SINK_H_

#include "a_data_source.h"  //ListenerKey typedef
#include <memory> // shared_ptr

// ListenerKey typedef is in a_data_source.h

class BoolDefaultsFalse{
 public:
  BoolDefautsFalse{
    my_state = false;
  }
  BoolDefaultsFalse(bool input_bool){
    my_state = input_bool;
  }
  bool operator()(){ return my_state };
 private:
  my_state;
}

class ADataSink;
typedef shared_ptr <ADataSink> DataSinkPtr;
typedef vector < DataSinkPtr > DataSinkList;

template < class DataSourceType, class DataStoreType >
class ADataSink{

 public:
 
  // derived classes override operator() to do sink-specific work
  // no need to do any thread stuff. (threads handled by
  // the data source and data_sink_worker) Just ask the data
  // source we own for data by data_source.get_data(&data);
  // then do whatever with it
  virtual operator()() = 0;
  
  DataSourceType *source_buffer;
  DataStoreType   data;

  static DataSinkList data_sink_list;

 protected:
  
  ADataSourcePtr my_data_source;

  void get_data_from_source();

  void register_sink();
  ListenerKey my_key;
  BoolDefaultsFalse registration_done;

};


template < class DataSourceType, class DataStoreType >
void ADataSink::register_sink(){

  data_sink_list.push_back( DataSinkPtr(this) );
  
  my_key = data_sink_list.size();
  
  my_data_source -> register_listener( my_key );

  registration_done(true);

}


// define the sink list
DataSinkList ADataSink::data_sink_list;

#endif
