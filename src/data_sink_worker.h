#ifndef DATA_SINK_WORKER_H_
#define DATA_SINK_WORKER_H_

#include <memory> //std::shared_ptr
#include <vector>
#include "a_data_source.h"
#include "a_data_sink.h"

template <class Source, class Sink>
  class DataSinkWorker;

template <class Source, class Sink>
class DataSinkWorker{

 public:

  typedef typename Source::SourcePtr SourcePtr;;
  typedef typename Source::SourceList SourceList;
  typedef typename Sink::SinkPtr SinkPtr;
  typedef typename Sink::SinkList SinkList;
  //  typedef typename PtrTo<Sink>::Type SinkPtr;
  //typedef typename PtrTo<Sink>::ListType SinkList;
  typedef typename PtrTo<DataSinkWorker<Source,Sink> >::Type DataWorkerPtr;
  typedef typename PtrTo< DataSinkWorker<Source,Sink> >::ListType DataWorkerList;

  DataSinkWorker( SourcePtr         data_source,
		  SinkList          data_sink_list,
		  ArteGlobalState  &arte_global_state);

  void operator()();

  static DataWorkerList data_worker_list;

  static void init_all_workers ( SourceList   data_source_list,
				 SinkList     data_sink_list,
				 int n_workers_per_data_source );

  static void run_all_workers  ( SourceList      data_source_list,
				 SinkList        data_sink_list,
				 int    n_workers_per_data_source,
				 ArteGlobalState               &gs);

  
  SourcePtr            data_source;
  // http://stackoverflow.com/questions/892133/should-i-prefer-pointers-or-references-in-member-data
  // There is some controversy about references as member data
  // TODO: consider replacing them with pointers or smartpointers
  SinkList             &data_sink_list;
  std::mutex               &data_mutex;
  //  std::mutex               &data_registry_mutex;
  std::condition_variable  &data_ready_cond;
  std::stack <ListenerKey> &dirty_list;

  // Hold poitners to the grabbed sink.
  ListenerKey  this_key;
  SinkPtr      this_sink;
  
  ArteGlobalState &arte_global_state;

};

template <class Source, class Sink>
  typename DataSinkWorker<Source,Sink>::DataWorkerList  
  DataSinkWorker<Source,Sink>::data_worker_list;

template <class Source, class Sink>
  DataSinkWorker<Source,Sink>::DataSinkWorker( SourcePtr   data_source,
					       SinkList data_sink_list,
					       ArteGlobalState &arte_global_state )
  : data_source        (data_source)
  , data_sink_list     (data_sink_list)
  , data_mutex         ( (arte_global_state.data_mutex) )
  , data_ready_cond    ( (arte_global_state.data_ready_cond) )
  , dirty_list         ( (data_source->dirty_list) )
  , arte_global_state  ( arte_global_state )
{
  std::cout << "Data_sink_worker constructor\n";
}

template <class Source, class Sink>
  void  DataSinkWorker<Source,Sink>::operator()()
{

  std::cout << "is_acquiring = " << arte_global_state.is_acquiring() << std::endl;
  while ( arte_global_state.is_acquiring() ){

    { // smaller scope to hold data lock
      std::cout << "Worker asking for lock on data_mutex\n";
      std::unique_lock <std::mutex> data_lk (data_mutex);
      std::cout << "Worker got lock on data_mutex\n";
      while( data_source->data_is_valid == false ){
	std::cout << "Worker waiting on condition variable\n";
	data_ready_cond.wait (data_lk);      
      }
      std::cout << "Worker got past condition variable.\n";
      if(!dirty_list.empty() ){
	this_key = dirty_list.top();
	std::cout << "Worker this_key = " << this_key << std::endl;
	dirty_list.pop();
	if( dirty_list.empty() ){
	  data_source->data_is_valid = false;
	}
	std::cout << "Worker about to call sink()\n";
	(*data_sink_list[this_key])();
      }
    } // ending lock_guard scope unlocks data_mutex
    
    (*data_sink_list[this_key]) ();
  
  }
}

template <class Source, class Sink>
  void  DataSinkWorker<Source,Sink>::run_all_workers( SourceList       data_source_list,
						      SinkList           data_sink_list,
						      int     n_workers_per_data_source,
						      ArteGlobalState                &gs)
{
  // do I need this?  Need to hald a list of the threads,
  // in addition to a list of the workers?
  std::vector <std::thread*> thread_ptr_list;
  
  for(typename SourceList::iterator it = data_source_list.begin();
      it != data_source_list.end(); it++){
    for(int i = 0; i < n_workers_per_data_source; i++){
      std::cout << "Starting a thread\n";
      DataWorkerPtr this_worker ( new DataSinkWorker ( *it, data_sink_list, gs ) );
      data_worker_list.push_back( this_worker );
      std::cout << "About to call *this_worker()\n";;
      std::thread this_thread ( *this_worker );
      this_thread.detach();
    }
  }
}



#endif
