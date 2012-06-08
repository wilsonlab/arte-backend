#ifndef DATA_SINK_WORKER_H_
#define DATA_SINK_WORKER_H_

#include "a_data_source.h"
#include "a_data_sink.h"

class DataSinkWorker;
typedef shared_ptr <DataSinkWorker> DataWorkerPtr;
typedef vector <DataWorkerPtr> DataWorkerList;

class DataSinkWorker{

 public:
  // DataSinkWorker needs to know what data source to look
  // to for the 'dirty_list' to pick from,
  // and needs the list of data_sinks referenced by that list
  DataSinkWorker( ADataSourcePtr data_source,
		  DataSinkList&  data_sink_list );

  void operator()();

  static DataWorkerList data_worker_list;
  static void init_all_workers ( ADataSourceList &data_source_list,
				 ADataSinkList   &data_sink_list,
				 int n_workers_per_data_source );

 private:
  
  ADataSourcePtr            data_source;
  // http://stackoverflow.com/questions/892133/should-i-prefer-pointers-or-references-in-member-data
  // There is some controversy about references as member data
  // TODO: consider replacing them with pointers or smartpointers
  DataSinkList             &data_sink_list;
  std::mutex               &data_mutex;
  //  std::mutex               &data_registry_mutex;
  std::condition_variable  &data_ready_cond;
  std::stack <ListenerKey> &dirty_list;

  // Hold poitners to the grabbed sink.
  ListenerKey  this_key;
  ADataSinkPtr this_sink;
  
  ArteGlobalState &arte_global_state;

};

DataWorkerList DataSinkWorker::data_worker_list;

#endif
