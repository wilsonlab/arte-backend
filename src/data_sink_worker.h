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

  typedef std::shared_ptr <DataSinkWorker <Source, Sink> > WorkerPtr;
  typedef std::vector <WorkerPtr> WorkerList;

  DataSinkWorker( std::stack <typename Sink::SinkPtr>      &shared_dirty_list,
		  std::mutex                               &data_mutex,
		  std::condition_variable                  &data_cond,
		  bool                                     *pool_smudge_addy,
		  bool                                     *acquiring );


  ~DataSinkWorker() { std::cout << "WORKER DESTRUCTOR\n"; }

  void operator()();

  //typename Source::SourcePtr                this_source;
  int id;
  void print_dirty_list();
  typename Sink::SinkPtr                  this_sink;
  
  std::stack <typename Sink::SinkPtr>                      &shared_dirty_list;

  // Dependency injection: pool's mutex and condition variable are passed in
  std::mutex               &pool_list_mutex;
  std::condition_variable  &data_ready_cond_var;
  bool                     *acquiring;
  bool                     *pool_smudge_addy;

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
  void register_self_with_sources();

 private:
  
  void refresh_dirty_list();
  typename Sink::SinkList                    data_sink_list;  
  std::stack <typename  Sink::SinkPtr >      dirty_list;
  std::mutex                                 pool_list_mutex;
  std::condition_variable                    data_ready_cond_var;
  //  std::mutex                                 workers_mutex;
  //  std::condition_variable                    workers_cond;
  ArteGlobalState                            *global_state_p;
  bool                                       acquiring;
  int                                        n_workers;
  bool                                       pool_smudge;
  bool                                       *pool_smudge_addy;
};


template <class Source, class Sink>
  WorkerPool <Source,Sink>::WorkerPool( typename Sink::SinkList  data_sink_list,
					int                           n_workers,
					ArteGlobalState                     *gs)
    : data_sink_list  (data_sink_list)
    , n_workers       (n_workers)
    , global_state_p  (gs)
				      //    , pool_mutex      (gs->data_mutex)
				      //    , pool_ready_cond (gs->data_ready_cond)
{
  std::cout << "WorkerPool Constructor\n";
  pool_smudge_addy = &pool_smudge;
  register_self_with_sources();
}

template <class Source, class Sink>
  void WorkerPool <Source, Sink>::register_self_with_sources(){
  std::set < typename Source::SourcePtr > source_set;
  for (auto it = data_sink_list.begin(); it != data_sink_list.end(); it++)
    source_set.insert( (*it) -> my_data_source );

  for (auto it = source_set.begin(); it != source_set.end(); it++)
    (*it) -> register_listener ( pool_smudge_addy, &data_ready_cond_var );
  
}


template <class Source, class Sink>
void  WorkerPool <Source, Sink>::run()
{

  acquiring = global_state_p -> is_acquiring();
  std::vector <std::thread> thread_list;
  typename DataSinkWorker<Source,Sink>::WorkerList worker_list;
  std::cout << "POOL about to create workers\n";
  for (int i = 0; i < n_workers; i++ ){
    //DataSinkWorker<Source,Sink> this_worker (dirty_list, workers_mutex, workers_cond, &acquiring );
    //thread_list.push_back( std::thread ( &DataSinkWorker<Source,Sink>::operator(), &this_worker ) );

    
    worker_list.push_back( typename DataSinkWorker<Source,Sink>::WorkerPtr( new DataSinkWorker<Source,Sink> (dirty_list, pool_list_mutex, data_ready_cond_var, pool_smudge_addy, &acquiring)));
    // worker_list.push_back( typename DataSinkWorker<Source,Sink>::WorkerPtr( new DataSinkWorker<Source,Sink> (dirty_list, pool_mutex, workers_cond, &acquiring)));
    thread_list.push_back( std::thread ( &DataSinkWorker<Source,Sink>::operator(), (worker_list[i]) ) );
    worker_list[i]->id = i;
    //    thread_list.push_back( std::thread ( DataSinkWorker<Source,Sink> (dirty_list, workers_mutex, workers_cond, &acquiring) ));


  }
  std::cout << "POOL finished making workers\n";

  while( global_state_p -> is_acquiring() ){

    { // small scope for list_mutex
      std::unique_lock <std::mutex> list_lock (pool_list_mutex);
      while( !pool_smudge ){
	data_ready_cond_var.wait(list_lock);
	std::cout << "POOL waited for data_ready_cond_var and got through.\n";
      }
      //      std::cout << "POOL: Got past data_ready_cond_var, refreshing dirty_list\n";
      refresh_dirty_list();
    }

  }

  for (auto it = thread_list.begin(); it != thread_list.end(); it++)
    it->join();

  }


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

  //  std::cout << "POOL: Refreshed Dirty List to: ";
  //    print_dirty_list();
  
}

template <class Source, class Sink>
  void WorkerPool<Source, Sink>::print_dirty_list(){
  std::stack <typename Sink::SinkPtr> dirty_list_copy (dirty_list);
  std::string list_string ("POOL: dirty_list: ");
  char buff[20];
  //std::cout << "POOL: dirty_list:";
  while( !dirty_list_copy.empty() ){
    sprintf(buff," %p  ", &(*dirty_list_copy.top()) );
    list_string += buff;
    //    std::cout << " " << dirty_list_copy.top() << " |";
    dirty_list_copy.pop();
  }
  list_string += '\n';
  std::cout << list_string;
}

template <class Source, class Sink>
  void DataSinkWorker<Source, Sink>::print_dirty_list(){
  std::stack <typename Sink::SinkPtr> dirty_list_copy (shared_dirty_list);
  std::string list_string ("POOL: dirty_list: ");
  char buff[20];
  //std::cout << "POOL: dirty_list:";
  while( !dirty_list_copy.empty() ){
    sprintf(buff," %p  ", &(*dirty_list_copy.top()) );
    list_string += buff;
    //    std::cout << " " << dirty_list_copy.top() << " |";
    dirty_list_copy.pop();
  }
  list_string += '\n';
  std::cout << list_string;
}

template <class Source, class Sink>
  DataSinkWorker<Source,Sink>::DataSinkWorker( std::stack <typename Sink::SinkPtr>      &shared_dirty_list,
					       std::mutex                               &pool_list_mutex,
					       std::condition_variable                  &data_ready_cond_var,
					       bool                                     *pool_smudge_addy,
					       bool                                     *acquiring )
    : shared_dirty_list    (shared_dirty_list)
    , pool_list_mutex      (pool_list_mutex)
    , data_ready_cond_var  (data_ready_cond_var)
    , pool_smudge_addy     (pool_smudge_addy)
    , acquiring            (acquiring)
{};

template <class Source, class Sink>
  void  DataSinkWorker<Source,Sink>::operator()(){
  bool new_data;
  while ( *acquiring ){

    { // smaller scope for data-copy mutex
      
      new_data = false;
      
      std::unique_lock <std::mutex> lk (pool_list_mutex);
      while( ! (*pool_smudge_addy) ){
	data_ready_cond_var.wait(lk);
      }
      
      //      printf("WORKER %d About to test shared_smudge_list empty\n", id);
      if( ! shared_dirty_list.empty() ){
	printf("WORKER %d Got into smudge list while loop\n", id);
	
	//	std::unique_lock <std::mutex> data_lk (data_mutex);
	this_sink = shared_dirty_list.top();
	printf("WORKER %d about to process sink addy %p\n", id, &(*this_sink));
	shared_dirty_list.pop();
	this_sink->get_data_from_source();
	this_sink->set_smudge(false);  // <-- should this be done by get_data_from_source?
	new_data = true;

	if ( shared_dirty_list.empty() ){
	  printf("WORKER %d took the last data and is de-smudging the pool.\n", id);
	  *pool_smudge_addy = false;
	}

      } // done copying data, release the data lock, do processing

    }

      if(new_data)
	(*this_sink)();

  }
  std::cout << "WORKER fell through while loop\n";
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
