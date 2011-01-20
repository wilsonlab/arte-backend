#include <nidaqmx.h>
#include <stdint.h>
#include <string>
class Timer {

 public:

  Timer(string deviceID);

  virtual ~Timer();

  int startTimer();
  int stopTimer();

  int setInitCount(uint16_t initCount);
  uint32_t getCount();
  uint64_t getTimestamp();

  int setTimerID(int8_t newId);
  int8_t getTimerID();

  const int N_MASTER_SYNC_TX = 10;
  const int N_SLAVE_SYNC_TX =1;

 private:

  //Methods
  int txSyncCount(uint32_t syncCount, int nPacket);
  uint32_t rxSyncCount();
  int becomeMaster();
  int armTimer();

  int initDAQCounterTask();
  int initDAQSyncTask();
  int armCounterTask();
 
  //naming syntax required for the  NIDaqMx signalChange
  int32 CVICALLBACK digitalChangeCallback(TaskHandel taskHandle, int32 signalID, void *callbackData);

 //Variables
  string niDevice;
  int8_t timerId;
 
  bool running;
  bool armed;
  bool master; 
  bool isSynced;

  uint32_t initCount;
  int countDir;
  int countEdge;

  int txSyncPort;
  int rxSyncPort;

  uint32_t prevSyncCount;
  uint32_t curSyncCount;

  TaskHandle counterTask;
  TaskHandle diPulseTask;

}
