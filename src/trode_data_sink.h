#ifndef TRODE_DATA_SINK_H_
#define TRODE_DATA_SINK_H_

#include "arte_pb.pb.h"
#include "arte_command.pb.h"
#include "a_data_sink.h"
#include "filt.h"
#include "netcom.h"

class TrodeDataSink : public ADataSink<NeuralVoltageBuffer,NeuralVoltageCircBuffer >
{

 public:

  TrodeDataSink( ArteTrodeOptPb &trode_opt );
  TrodeDataSink( ArteTrodeOptPb &t_o, ArteTrodeOptPb &d_o, Filt::FiltList &filter_list );
  void get_data_from_source();
  void operator()();
  void handle_command( ArteCommand &the_command );

 private:
  
  int id;

  // DataSource also declared in a_data_sink.h, but must
  // be assigned in TrodeDataSink constructor
  // data and source_buffer are declared in a_data_sink.h

  std::shared_ptr <oGlom>  main_file;
  std::shared_ptr <NetCom> data_netcom;

  ArteTrodeOptPb trode_opt;

  ArtePb      parent_pb;
  ArteSpikePb *spike_pb;

  // why does trode_data_sink have this?
  ArteVoltageTimeseries voltages_pb;

  // I don't make a default trode, just keep the default settings in pb
  //static ADataSinkPtr default_trode;

  Filt filter;

  int n_samps, n_chans;
  NeuralVoltageCircBuffer filtered_data;

  void scan_for_spikes();
  void publish_spikes();

  timestamp_t last_spike_time;

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

  int spike_seek_start;
  int spike_seek_end;

  std::vector <int> spike_index;

};


#endif
