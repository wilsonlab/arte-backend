#ifndef DATA_SINK_WORKER_H_
#define DATA_SINK_WORKER_H_

#include <memory> //std::shared_ptr
#include <vector>
#include "a_data_source.h"
#include "a_data_sink.h"

template <class Source, class Sink>
  class WorkerPool;



template <class Source, class Sink>
class DataSinkWorker{

 public:
  friend class WorkerPool < Source, Sink >;

  DataSinkWorker( std::stack <typename Sink::SinkPtr>      &shared_dirty_list,
		  std::mutex                               &data_mutex,
		  std::condition_variable                  &data_cond,
		  bool                                     *acquiring );


  ~DataSinkWorker() { std::cout << "WORKER DESTRUCTOR\n"; }

  void operator()();

  //typename Source::SourcePtr                this_source;
  typename Sink::SinkPtr                  this_sink;
  
  std::stack <typename Sink::SinkPtr>                      &shared_dirty_list;

  // Dependency injection: pool's mutex and condition variable are passed in
  std::mutex               &data_mutex;
  std::condition_variable  &data_ready_cond;
  bool                     *acquiring;

};


template <class Source, class Sink>
  class WorkerPool{

 public:
  WorkerPool (typename Sink::SinkList     data_sink_list,
	      int                         n_workers,
	      ArteGlobalState             *gs               );
  friend class DataSinkWorker< Source, Sink >;
  void run();
  void update_dirty_list();
  void print_dirty_list();


 private:
  
  void refresh_dirty_list();
  typename Sink::SinkList                    data_sink_list;  
  std::stack <typename  Sink::SinkPtr >      dirty_list;
  std::mutex                                 &pool_mutex;
  std::condition_variable                    &pool_ready_cond;
  std::mutex                                 workers_mutex;
  std::condition_variable                    workers_cond;
  ArteGlobalState                            *global_state_p;
  bool                                       acquiring;
  int                                        n_workers;
};


template <class Source, class Sink>
  WorkerPool <Source,Sink>::WorkerPool( typename Sink::SinkList  data_sink_list,
					int                           n_workers,
					ArteGlobalState                     *gs)
    : data_sink_list  (data_sink_list)
    , n_workers       (n_workers)
    , global_state_p  (gs)
    , pool_mutex      (gs->data_mutex)
    , pool_ready_cond (gs->data_ready_cond)
{
  std::cout << "WorkerPool Constructor";
}


template <class Source, class Sink>
void  WorkerPool <Source, Sink>::run()
{

  acquiring = global_state_p -> is_acquiring();
  std::vector <std::thread> thread_list;
  for (int i = 0; i < n_workers; i++ ){
    DataSinkWorker<Source,Sink> this_worker (dirty_list, workers_mutex, workers_cond, &acquiring );

    /*
    std::thread this_thread ( (this_worker) );
    //    std::thread this_thread( DataSinkWorker<Source,Sink> ( dirty_list, workers_mutex, workers_cond, &acquiring ) );
    thread_list.push_back( this_thread );
    */

    thread_list.push_back( std::thread ( this_worker ) );

  }

  std::cout << "POOL About to get to While loop and gs->acquiring is: " << global_state_p->is_acquiring() << std::endl;

  while ( global_state_p -> is_acquiring() ){
    // update local copy of acquiring for the workers to read
    acquiring = global_state_p -> is_acquiring();
    std::unique_lock <std::mutex> pool_lk (pool_mutex);
    refresh_dirty_list();
    print_dirty_list();
    { // sub scope for worker mutex
      while( dirty_list.empty() ){
	refresh_dirty_list();
	pool_ready_cond.wait( pool_lk );
	std::cout << "POOL got past cond variable\n";
      }

      refresh_dirty_list();
      acquiring = global_state_p -> is_acquiring();

      std::unique_lock <std::mutex> workers_lk ( workers_mutex );
      workers_cond.notify_all();
	
    } // end sub scope for worker mutex
  } // end while block

  std::cout << "POOL got past while loop\n";

  // Done acquiring, so collect up the worker threads
  for (auto it = thread_list.begin(); it != thread_list.end(); it++)
    it->join();

};


template <class Source, class Sink>
void  WorkerPool<Source, Sink>::refresh_dirty_list()
{
  while( !dirty_list.empty() )
    dirty_list.pop();

  for (auto it = data_sink_list.begin(); it != data_sink_list.end(); it++){
    if ( (*it)->read_smudge() ){
      dirty_list.push( *it );
    }
  }
}

template <class Source, class Sink>
  void WorkerPool<Source, Sink>::print_dirty_list(){
  std::stack <typename Sink::SinkPtr> dirty_list_copy (dirty_list);
  std::cout << "POOL: dirty_list:";
  while( !dirty_list_copy.empty() ){
    std::cout << " " << dirty_list_copy.top() << " |";
    dirty_list_copy.pop();
  }
  std::cout << std::endl;
}

/*

template <class Source, class Sink>
  DataSinkWorker<Source,Sink>::DataSinkWorker( std::stack <typename Source::SourcePtr>  dirty_list,
					       std::mutex                               &data_mutex,
					       std::condition_variable                  &data_cond
					       bool                                     &acquiring)

    : data_mutex         ( (arte_global_state->data_mutex) )
    , data_ready_cond    ( (arte_global_state->data_ready_cond) )
    , dirty_list         ( (data_source->dirty_list) )
    , acquiring          ( acquiring )
{
}
*/

template <class Source, class Sink>
  DataSinkWorker<Source,Sink>::DataSinkWorker( std::stack <typename Sink::SinkPtr>      &shared_dirty_list,
					       std::mutex                               &data_mutex,
					       std::condition_variable                  &data_cond,
					       bool                                     *acquiring )
    : shared_dirty_list (shared_dirty_list)
    , data_mutex        (data_mutex)
    , data_ready_cond   (data_cond)
    , acquiring         (acquiring)
{};

template <class Source, class Sink>
  void  DataSinkWorker<Source,Sink>::operator()(){
  while ( *acquiring ){

    { // smaller scope to hold data lock
      std::unique_lock <std::mutex> data_lk (data_mutex);
      while( !shared_dirty_list.empty() ){
	std::cout << "WORKER waiting for data cond variable\n";
	data_ready_cond.wait (data_lk);      
      }
      std::cout << "WORKER about to process data\n";
      this_sink = shared_dirty_list.top();
      shared_dirty_list.pop();
      std::cout << "WORKER about to tell sink to get_data_from_source();\n";
      this_sink->get_data_from_source();
      this_sink->set_smudge(false);
      
      //  if(!dirty_list.empty() ){

	//	this_key = dirty_list.top();
	//dirty_list.pop();
	//if( dirty_list.empty() ){
	//  data_source->data_is_valid = false;
	//}
	//(*data_sink_list[this_key])();

	//}
    }// ending lock_guard scope unlocks data_mutex
    std::cout << "WORKER About to call Sink()\n";
    (*this_sink) ();

    //    (*data_sink_list[this_key]) ();
  
  }
};


/*
template <class Source, class Sink>
  void  DataSinkWorker<Source,Sink>::run_all_workers( SourceList       data_source_list,
						      SinkList           data_sink_list,
						      int     n_workers_per_data_source,
						      ArteGlobalState                *gs)
{

  std::vector <std::thread> thread_list;
  
  for(typename SourceList::iterator it = data_source_list.begin();
      it != data_source_list.end(); it++){
    for(int i = 0; i < n_workers_per_data_source; i++){

      DataWorkerPtr this_worker (new DataSinkWorker (*it, data_sink_list, gs) );
      thread_list.push_back (std::thread ( &DataSinkWorker<Source,Sink>::operator(), this_worker ) );
      // For some reason, detach() instead of join() seems to turn data_sink_list to nonsense (size() = garbage)
      // Also causes segfalt
      // Oh - it's because thread_list is temporary?  Leaving scope closes all the threads in the list?
      // But why would that cause segfalt?
    }
  }

  for(auto t = thread_list.begin(); t!= thread_list.end(); t++)
    t->join();

}
*/


#endif
