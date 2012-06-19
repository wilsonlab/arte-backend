#include <stdio.h>
#include <iostream>
#include <thread>
#include <algorithm>
#include <memory>
#include "../a_data_source.h"
#include "../a_data_sink.h"
#include "../arte_pb.pb.h"
#include "../global_state.h"
#include "../nidaq_data_source.h"
#include "../data_sink_worker.h"

bool acquiring = false;

class MockDataSource : public ADataSource<double>{

public:

  typedef std::shared_ptr <MockDataSource> MockDataSourcePtr;
  typedef std::vector <MockDataSourcePtr> SourceList;
  
  MockDataSource(ArteGlobalState &global_state){
    internal_count = 100.0;
    my_id = source_count;
    source_count++;
    global_state_p = &global_state;
    std::cout << "Source " << my_id << " in constructor.\n";
  }
  ~MockDataSource(){
    std::cout << "Source " << my_id << " in destructor.\n";
  }
  void operator()(){ 
    //std::cout << "Source got into operator()()\n";
    global_state_p->acquiring = true;
    while(global_state_p->is_acquiring()){
      internal_count++;
      //std::cout << "Source in while loop because acquiring = " 
	//	<< global_state_p->is_acquiring() << std::endl;
      //      std::cout << "Source set count to internal_count=" << internal_count << std::endl;
      set_data(internal_count);
      sleep(1);
    }
    std::cout << "MOCKSOURCE: Got past while (acquiring) loop.\n";
  }

    void start() {
      //    (*this)();
  }
  void stop() {
  }

  int my_id;
  static int source_count;

private:
  double internal_count;              

};
int MockDataSource::source_count = 0;

class MockDataSink;
typedef std::shared_ptr<MockDataSink> MockDataSinkPtr;
typedef std::shared_ptr<MockDataSource> MockDataSourcePtr;


class MockDataSink : public ADataSink <double, double>{
public:
  MockDataSink() { std::cout << "Derived no args constructor\n"; fflush(stdout); }
  MockDataSink( MockDataSourcePtr the_source, ArteGlobalState *gs )
    :ADataSink<double,double> (the_source, gs)
  {}
  //  void get_data_from_source() { data = 100 + my_data_source->get_data(); }
  void operator()(){
    //    get_data_from_source();
    //std::cout << "SINK: operator();\n"; fflush(stdout);
    for(int i = 0; i < 4000000; i++)
      data = 1 + (sqrt( 1 + sqrt( 1 + sqrt( 200 + data))));
    printf("Sink data: %f\n", data);
  }

};



int main( int argc, char *argv[]){

  ArteGlobalState gs;
  MockDataSource::SourceList source_list;
  /*
  MockDataSourcePtr source = MockDataSourcePtr (new MockDataSource (gs) );

  source_list.push_back(source);
  std::thread acq ( &MockDataSource::operator(), source );
  //std::cout << "source = " << source << std::endl;
  */

  source_list.push_back( MockDataSourcePtr (new MockDataSource (gs) ) );

  MockDataSink::SinkList sink_list;  
  for(int i = 0; i < 10; i++){
    sink_list.push_back (MockDataSinkPtr( new MockDataSink(  source_list[0], &gs ) ));
  }  

  std::thread acq ( &MockDataSource::operator(), source_list[0] );


  WorkerPool<MockDataSource,MockDataSink> my_pool( sink_list, 100, &gs );
  std::cout << "Just finished constructing pool\n";
  sleep(1);
  my_pool.run();
  std::cout << "Just finished call to my_pool.run()\n";
  sleep(1);



  //ompdo_work.detach();

  std::cout << "Enter a number and hit return to stop acquiring.\n";
  int m;
  std::cin >> m;
  gs.acquiring = false;

  acq.join();    
  std::cout << "sleep(2) and then quit\n";
  sleep(2);

  std::cout << "Reached end of main. About to return\n";

  return 0;

}


