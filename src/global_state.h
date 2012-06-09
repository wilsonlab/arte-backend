#ifndef GLOBAL_STATE_H_
#define GLOBAL_STATE_H_

#include <thread>
#include <mutex>

class ArteGlobalState{
  
 public:
  std::mutex                    data_mutex;
  std::condition_variable  data_ready_cond;


};

#endif
