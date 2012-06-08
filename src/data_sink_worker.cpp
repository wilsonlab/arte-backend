#include "data_sink_worker.h"

DataSinkWorker::DataSinkWorker( ADataSourcePtr   data_source,
				DataSinkList    &data_sink_list,
				ArteGlobalState &arte_global_state )
  : data_source (data_source)
  , data_sink_list (data_sink_list)
  , data__mutex ( &(data_source->data_mutex) )
  , data_ready_cond   ( &(data_source->data_ready_cond) )
  , dirty_list        ( &(data_source->dirty_list) )
  , global_state_ref  ( arte_global_state )
{}

DataSinkWorker::operator()()
{

  while ( arte_global_state.acquiring() ){

    { // smaller scope to hold data lock
      lock_guard <std::mutex> data_lk (data_mutex);

      while( data_source->data_is_valid == false ){
	data_ready_cond.wait (data_lk, []);      
      }
      
      if(!dirty_list.empty() ){
	this_key = dirty_list.top();
	dirty_list.pop();
	if( dirty_list.empty() ){
	  data_source->data_is_valid = false;
	}
	data_sink_list[this_key].get_data();
      }
    } // ending lock_guard scope unlocks data_mutex
    
    data_sink_list[this_key] ();
  
  }
}

DataSinkWorker::run_all_workers( ADataSourceList &data_source_list,
				 DataSinkList      &data_sink_list,
				 int     n_workers_per_data_source )
{
  // do I need this?  Need to hald a list of the threads,
  // in addition to a list of the workers?
  std::vector <std::thread*> thread_ptr_list;

  for(DataSourceList::iterator it = data_source_list.begin();
      it != data_source_list.end(); it++){
    for(int i = 0; i < n_workers_per_data_source; i++){
      DataWorkerPtr this_worker ( new DataSinkWorker ( *it, data_sink_list ) );
      data_worker_list.push_back( tmp );
      std::thread this_thread ( this_worker );
      this_thread.detach();
    }
  }
}
