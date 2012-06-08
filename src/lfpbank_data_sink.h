#ifndef LFPBANK_DATA_SINK_H_
#define LFPBANK_DATA_SINK_H_

#include "a_data_sink.h"

class LfpbankDataSink : public ADataSink<NeuralVoltageBuffer,NeuralVoltageCircBuffer>
{

 public:

  LfpbankDataSink( ArteLfpOptPb &lfp_opt );
  operator()();

 private:
  
  std::shared_ptr <oGlom> main_file;

  ArteLfpOptPb lfp_opt;

  ArteLfpPb lfp_pb;
  ArteVoltageTimeseries voltages_pb;

  static LfpbankDataSink default_lfp;

  Filter filter;

  int lfp_opt_version;
  std::vector <double> input_gain;
  int daq_id;
  std::vector <int> daq_chans;
  int keep_nth_sample;
  std::string port;
  std::string host_ip;

  bool disk;
  bool network;  

};

#endif
