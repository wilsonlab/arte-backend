#ifndef A_DATA_SINK_H_
#define A_DATA_SINK_H_

#include <memory> // shared_ptr
#include <boost/variant.hpp>
#include <boost/bind.hpp>
#include "a_data_source.h"  //ListenerKey typedef
#include "nidaq_data_source.h"


// ListenerKey typedef is in a_data_source.h

class BoolDefaultsFalse{
 public:
  BoolDefaultsFalse(){
    my_state = false;
  }
  void operator()(bool input_bool){
    my_state = input_bool;
  }
  bool operator()(){ return my_state; }
 private:
  bool my_state;
};

class TrodeDataSink;
typedef std::shared_ptr <TrodeDataSink> TrodeDataSinkPtr;
class LfpbankDataSink;
typedef std::shared_ptr <LfpbankDataSink> LfpbankDataSinkPtr;
class MockDataSink;
typedef std::shared_ptr <MockDataSink> MockDataSinkPtr;

typedef boost::variant <TrodeDataSinkPtr, LfpbankDataSinkPtr, MockDataSinkPtr> AnyDataSink;
typedef std::vector <AnyDataSink> DataSinkList;


template < class DataSourceType, class DataStoreType >
class ADataSink{

 public:
 
  // derived classes override operator() to do sink-specific work
  // no need to do any thread stuff. (threads handled by
  // the data source and data_sink_worker) Just ask the data
  // source we own for data by data_source.get_data(&data);
  // then do whatever with it
  virtual void operator()();
  
  DataSourceType *source_buffer;
  DataStoreType   data;

 protected:
  
  ADataSource<DataSourceType> my_data_source;

  void get_data_from_source();

  void register_self( AnyDataSink this_wrapped );
  ListenerKey my_key;
  BoolDefaultsFalse registration_done;

  static DataSinkList data_sink_list;

};


class RegisterVisitor : boost::static_visitor<>{
 public:
  template <class T>
    std::stack <ListenerKey> & operator()( T& this_data_sink, int the_key) const
    {
      // TODO: figure out - why am I allowed to do this with nidaq_data_source.h
      // included, and not without?  Isn't MockDataSource also declaced but
      // not defined, at this point?  The problem seems to be that nidaq_data_source
      // is forward declared only?
      this_data_sink->register_listener( the_key );
    }
};

template < class DataSourceType, class DataStoreType >
  void ADataSink <DataSourceType, DataStoreType>::register_self( AnyDataSink wrapped_this ){
  
  
  data_sink_list.push_back( wrapped_this );
  
  my_key = data_sink_list.size();

  boost::apply_visitor( boost::bind( RegisterVisitor(), _1, my_key ), my_data_source );

  registration_done(true);

};


// define the sink list
template <class DataSourceType, class DataStoreType>
  DataSinkList ADataSink<DataSourceType, DataStoreType>::data_sink_list;

#endif
