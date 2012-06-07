#ifndef A_DATA_SINK_H_
#define A_DATA_SINK_H_

#include "a_data_source.h"  //ListenerKey typedef
#include <memory> // shared_ptr

// ListenerKey typedef is in a_data_source.h

class ADataSink;
typedef shared_ptr <ADataSink> DataSinkPtr;
typedef vector < DataSinkPtr > DataSinkList;

template < class SourceData, class StoredData >
class ADataSink{

 public:
  
  
}
