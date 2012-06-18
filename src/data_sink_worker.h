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
    (*it) -> register_listener ( pool_smudge_addy, data_ready_cond_var );
  
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

    
    worker_list.push_back( typename DataSinkWorker<Source,Sink>::WorkerPtr( new DataSinkWorker<Source,Sink> (dirty_list, pool_list_mutex, data_ready_cond_var, &acquiring)));
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
      }
      std::cout << "POOL: Got past data_ready_cond_var, refreshing dirty_list\n";
      refresh_dirty_list();
    }



  /* OLD WAY
  //  std::cout << "POOL About to get to While loop and gs->acquiring is: " << global_state_p->is_acquiring() << std::endl;
  printf("POOL About to get to while loop and gs->acquiring is: %d\n", global_state_p->is_acquiring());

  while ( global_state_p -> is_acquiring() ){
    // update local copy of acquiring for the workers to read
    acquiring = global_state_p -> is_acquiring();
    std::unique_lock <std::mutex> pool_lk (pool_mutex);
    refresh_dirty_list();
    print_dirty_list();
    { // sub scope for worker mutex

      std::unique_lock <std::mutex> workers_lk ( workers_mutex );
      std::cout << "POOL about to encounter while loop and ";
      refresh_dirty_list();
      while( dirty_list.empty() ){
	std::cout << "POOL in while( list.empty() ): ";
	refresh_dirty_list();
	pool_ready_cond.wait( pool_lk );
	std::cout << "POOL got past cond variable\n";
      }

      //refresh_dirty_list();
      acquiring = global_state_p -> is_acquiring();
      std::cout << "POOL trying to get lock on workers_mutex\n";

      //std::cout << "POOL calling notify_all()\n";
      //      workers_cond.notify_all();
	
      
      while( !dirty_list.empty() ){
	std::cout << "POOL waiting for dirty_list to empty.";
	std::unique_lock
	workers_cond.notify_one();


    } // end sub scope for worker mutex
  } // end while block

  std::cout << "POOL got past while loop\n";
  */ 
  // Done acquiring, so collect up the worker threads
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
    print_dirty_list();
  
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
					       bool                                     *acquiring )
    : shared_dirty_list    (shared_dirty_list)
    , pool_list_mutex      (pool_list_mutex)
    , data_ready_cond_var  (data_ready_cond_var)
    , acquiring            (acquiring)
{};

template <class Source, class Sink>
  void  DataSinkWorker<Source,Sink>::operator()(){
  while ( *acquiring ){

    { // smaller scope for data-copy mutex
      std::unique_lock <std::mutex> lk (pool_list_mutex);
      while( ! (*data_smudge_addy) ){
	data_ready_cond_var.wait(lk);
      }

      printf("WORKER %d About to test shared_smudge_list empty\n", id);
      if( ! shared_smudge_list.empty() ){
	printf("WORKER %d Got into smudge list while loop\n", id);
	
	std::unique_lock <std::mutex> data_lk (data_mutex);
	this_sink = shared_dirty_list.top();
	printf("WORKER %d about to process sink addy %p\n", id, &(*this_sink));
	shared_dirty_list.pop();
	this_sink->get_data_from_source();
	this_sink->set_smudge(false);  // <-- should this be done by get_data_from_source?
	
	if ( shared_dirty_list.empty() ){
	  printf("WORKER %d took the last data and is de-smudging the pool.\n", id);
	  *pool_smudge_addy = false;
	}

      } // done copying data, release the data lock, do processing

      (*this_sink)();

    }

    /* OLD WAY
    { // smaller scope to hold data lock
      std::unique_lock <std::mutex> data_lk (data_mutex);
      while( shared_dirty_list.empty() ){
	std::cout << "WORKER waiting for data cond variable\n";
	data_ready_cond.wait (data_lk);      
      } 
      printf("WORKER %d got past cond var and sees dirty_list: ", id);
      print_dirty_list();
      printf("WORKER %d about to process sink addy %p\n", id, &(*(shared_dirty_list.top())));
      //      std::cout << "WORKER about to process sink addy:" << shared_dirty_list.top() << std::endl;
      
      this_sink = shared_dirty_list.top();
      shared_dirty_list.pop();
      //std::cout << "WORKER about to tell sink to get_data_from_source();\n";
      this_sink->get_data_from_source();
      this_sink->set_smudge(false);
      
    }// ending lock_guard scope unlocks data_mutex
    //std::cout << "WORKER About to call Sink()\n";
    (*this_sink) ();
    
    //    (*data_sink_list[this_key]) ();
    */


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
