#include "a_data_source.h"

ADataSource::ADataSource( boost::shared_ptr <ATimer> a_timer_p )
  : timer_p (a_timer_p)
{
  
}

ADataSource::start(){
}

ADataSource::stop(){

  boost::mutex::scoped_lock (data_mutex);
  data_is_valid = false;
  data_ready_cond.notify_all();

}

timestamp_t ADataSource::get_data_timestamp(){
  
