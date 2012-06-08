/** NidaqDataSource - Interface to a NIDaq card 32 analog in chans
 *  
 */

#ifndef NIDAQ_DATA_SOURCE_H_
#define NIDAQ_DATA_SOURCE_H_

#include <boost/multi_array.hpp>
#include <memory> // std::shared_ptr <>
#include <NIDAQmx.h>
#include <map>
#include "a_data_source.h"
#include "arte_pb.pb.h"
#include "global_defs.h"
#include "glom.h"  


class NidaqDataSource;
typedef std::shared_ptr <NidaqDataSource> NidaqSourcePtr;
typedef std::map <int, NidaqSourcePtr> DaqList;

class NidaqDataSource : public ADataSource <NeuralVoltageBuffer> {

 public:

 NidaqDataSource ();
 NidaqDataSource( ArteNeuralDaqOptPb &daq_opt_pb,
		   std::shared_ptr <aTimer> timer );
  // Destructor checks that destruction is safe, complains if not
  ~NidaqDataSource();

  void start();
  void stop();

  timestamp_t buffer_delay;

  
  void print_state();
  bool get_is_master();

  static DaqList daq_list;

  // Because nidaq cards are coordinated within one machine,
  // we need helper functions that process daqs in the right order
  
  static int get_master_daq_key();
  static NidaqSourcePtr get_master_daq();
  
  static void start_all_daqs();
  static void stop_all_daqs (); 

  const ArteNeuralDaqOptPb * get_daq_settings();
  static void get_all_daqs_settings( ArteSetupOptPb &setup_opt );

 private:

  int id;
  bool is_master;
  bool is_multiplexing;

  ArteNeuralDaqOptPb daq_opt;

  static bool destruction_is_safe;

  // will we need these even when using multi_array, or
  // are they going to be used once and forgotten, source
  // of bugs when setting/saving state?
  int n_samps_per_buffer;
  int n_chans;

  // nidaqmx-style callbacks for when cards produce data or stop
  static int32 CVICALLBACK EveryNCallback(TaskHandle task_handle, 
				   int32 everyNSamplesEventType,
				   uInt32 nSamples,
				   void *callbackData);
  static int32 CVICALLBACK DoneCallback(TaskHandle task_handle, 
				 int32 status,
				 void *callbackData);
  
  // members for writing daq data directly to a buffer;
  // in support of MockedNidaqDataSource, which will 
  // read buffer data from an iGlom
  bool write_to_disk;
  std::string out_filename;
  std::shared_ptr <oGlom> daq_o_glom;

  // members for nidaqmx
  TaskHandle task_handle;
  std::string dev_name;
  int size_bytes;
  static char clk_src[256];
  static float64 clkRate;
  static char trig_name[256];

  // Copied from util.h, util.cpp; these should be here b/c
  // they're nidaqmx specific (eventually will remove from util.h
  // and util.cpp so that those can build w/out nidaq headers)
  static  void nidaq_err_check( int32 error );
  static void nidaq_err_check( int32 error, TaskHandle *task_handle_array, 
			       int n_tasks );
  static int32 GetTerminalNameWithDevPrefix(TaskHandle taskHandle, 
					    const char terminalName[], 
					    char triggerName[]);


};

typedef std::shared_ptr <NidaqDataSource> NidaqSourcePtr;
typedef std::map <int, NidaqSourcePtr> DaqList;

//float64 NidaqDataSource::clkRate = 0;

#endif
