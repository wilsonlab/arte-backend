#ifndef TRODE_DATA_SINK_H_
#define TRODE_DATA_SINK_H_

#include "a_data_sink.h"

class TrodeDataSink : public ADataSink<NeuralVoltageBuffer,NeuralVoltageCircBuffer >
{

 public:

  TrodeDataSink( ArteTrodeOptPb &trode_opt );
  operator()();
  handle_command( ArteCommandPb &the_command );

 private:
  
  int id;

  // DataSource also declared in a_data_sink.h, but must
  // be assigned in TrodeDataSink constructor
  // data and source_buffer are declared in a_data_sink.h

  std::shared_ptr <oGlom> main_file;

  ArteTrodeOptPb trode_opt;

  ArteSpikePb spike_pb;
  ArteVoltageTimeseries voltages_pb;

  static TrodeDataSink default_trode;

  Filter filter;

  int trode_opt_version;
  std::vector <double> input_gain;
  std::vector <double> threshold;
  std::string daq_id;
  std::vector <int> daq_chans;
  int samps_before_trig;
  int samps_after_trig;
  int refractory_period_samps;

  std::string port;
  std::string host_ip;

  bool disk;
  bool network;

};


#endif
