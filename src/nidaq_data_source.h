/** NidaqDataSource - Interface to a NIDaq card 32 analog in chans
 *  
 */

#ifndef NIDAQ_DATA_SOURCE_H_
#define NIDAQ_DATA_SOURCE_H_

#include <boost/multi_array.hpp>
#include <memory> // std::shared_ptr <>
#include <nidaqmx.h>
#include "global_defs.h"

typedef boost::multi_array <rdata_t,2> raw_voltage_array;
typedef raw_voltage_array::index index;

// If we ever get a card other than nidaq, then we need
// to move this data type out into a header that other
// data_source's can use
struct neural_voltage_buffer{
  timestamp_t       buffer_timestamp;
  raw_voltage_array buffer_voltage;
}

class NidaqDataSource : ADataSource <neural_voltage_buffer> {

  typedef std::shared_ptr<NidaqDataSource> NidaqSourcePtr;
  typedef std::map <int, NidaqSourcePtr> DaqList;

 public:

  
  NidaqDataSource( ArteNeuralDaqOptPb &daq_opt_pb,
		   std::shared_ptr <aTimer> timer );
  // Destructor checks that destruction is safe, complains if not
  ~NidaqDataSource();

  void start();
  void stop();
  
  void print_state();
  bool get_is_master();

  static DaqList daq_list;

  // Because nidaq cards are coordinated within one machine,
  // we need helper functions that process daqs in the right order
  
  int NidaqDataSource::get_master_daq_key();
  NidaqSourcePtr NidaqDataSource::get_master_daq();
  
  void NidaqDataSource::start_all_daqs();
  void NidaqDataSource::stop_all_daqs (); 

 private:
   
  neural_voltage_buffer data;
  neural_voltage_buffer scratch_data;

  int id;
  bool is_master;

  std::shared_ptr <aTimer> timer;
  timestamp_t buffer_delay;

  static bool destruction_is_safe;

  // will we need these even when using multi_array, or
  // are they going to be used once and forgotten, source
  // of bugs when setting/saving state?
  int n_samps_per_buffer;
  int n_chans;

  // nidaqmx-style callbacks for when cards produce data or stop
  int32 CVICALLBACK EveryNCallback(TaskHandle task_handle, 
				   int32 everyNSamplesEventType,
				   uInt32 nSamples,
				   void *callbackData);
  int32 CVICALLBACK DoneCallback(TaskHandle task_handle, 
				 int32 status,
				 void *callbackData);
  
  // members for writing daq data directly to a buffer;
  // in support of MockedNidaqDataSource, which will 
  // read buffer data from an iGlom
  bool write_to_disk;
  static std::shared_ptr <oGlom> daq_o_glom;

  // members for nidaqmx
  TaskHandle task_handle;
  std::string dev_name;
  int size_bytes;
  static char clk_src[256];
  static float64 clkRate;
  static char trig_name[256];

};
