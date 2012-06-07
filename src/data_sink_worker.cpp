#include "data_sink_worker.h"

DataSinkWorker::DataSinkWorker( ADataSourcePtr   data_source,
				DataSinkList    &data_sink_list,
				ArteGlobalState &arte_global_state )
  : data_source (data_source)
  , data_sink_list (data_sink_list)
  , data_source_mutex ( &(data_source->data_mutex) )
  , data_ready_cond   ( &(data_source->data_ready_cond) )
  , dirty_list        ( &(data_source->dirty_list) )
  , global_state_ref  ( arte_global_state )
{}

DataSinkWorker::operator()()
{
  while ( arte_global_state.acquiring() ){
    unique_lock <std::mutex> lk (data_mutex);
    data_ready_cond.wait (lk);
    while( !dirty_list.empty() ){
      this_key = dirty_list.top();
      dirty_list.pop();
      data_sink_list[this_key] ();
    }
  }
}

DataSinkWorker::init_all_workers( ADataSourceList &data_source_list,
				  DataSinkList    &data_sink_list,
				  int n_workers_per_data_source )
{

  for(DataSourceList::iterator it = data_source_list.begin();
      it != data_source_list.end(); it++){
    for(int i = 0; i < n_workers_per_data_source; i++){
      DataWorkerPtr tmp ( new DataSinkWorker ( *it, data_sink_list ) );
      data_worker_list.push_back( tmp );
    }
  }
}
