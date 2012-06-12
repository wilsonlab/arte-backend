#ifndef GLOBAL_STATE_H_
#define GLOBAL_STATE_H_

#include <thread>
#include <mutex>

class ArteGlobalState{
  
 public:
  ArteGlobalState();
  std::mutex                    data_mutex;
  std::condition_variable  data_ready_cond;
  bool is_acquiring(){ return acquiring; }
  bool is_disking() { return disking; }
  bool is_networking() {return networking; }


  bool acquiring;
  bool disking;
  bool networking;

};

#endif
