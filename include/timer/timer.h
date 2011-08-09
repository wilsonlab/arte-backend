#ifndef TIMER_H_
#define TIMER_H_

// isToy option makes this a dummy counter for testing of other arte classes
#define ISTOY true

#include <NIDAQmx.h>
#include <stdint.h>
#include <string>
#include "netcom.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

class Timer {

 private:

  //Methods
  int becomeMaster();
  int armTimer();

  int initDAQCounterTask();
  int initDAQSyncTask();
  int armCounterTask();
 
  //naming syntax required for the  NIDaqMx signalChange
//  int32 CVICALLBACK digitalChangeCallback(TaskHandle taskHandle, int32 signalID, void *callbackData);

 //Variables

  char niDev[256];
  char niCtr[256];
  char niArm[256];
  char niSync[256];

  int timerID;
 
  bool running;
  bool armed;
  bool master; 
  bool isSynced;

  bool ad_slave;
  uint16_t timer_freq;

  uint32_t initCount;
  int countDir;
  int countEdge;

  int txSyncPort;
  int rxSyncPort;

  uint32_t syncCount;

  TaskHandle counterTask;
  TaskHandle diPulseTask;

  TaskHandle counter_generation_task;
  TaskHandle counter_count_task;

  NetComDat txDat;
  NetComDat rxDat;

 public:
  Timer();

  virtual ~Timer();

  int txSyncCount(uint32_t syncCount, int nPacket);
  uint32_t rxSyncCount();

  bool isMaster();
  int becomeMaster(bool isMaster);
  int setDevStrs(std::string const& dev, std::string const& ctr, 
		 std::string const& arm, std::string const& sync);
 
  int start();
  int stop();
  void stop2();

  int setInitCount(uint32_t initCount);
  uint32_t getCount();
  uint32_t getTimestamp();

  int setTimerID(int newId);
  int getTimerID();

  uint32_t getSyncCount();
  int 	setSyncCount(uint32_t count);

  void initUdpTx(std::string host, int port);
  void initUdpRx(std::string host, int port);

  void init2(boost::property_tree::ptree &timer_pt);
  
  //const bool isToy = ISTOY;
  uint32_t toy_timestamp; //public settable fake timestamp
  uint32_t next_ok_ts_to_print;
};

#endif
