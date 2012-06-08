#include "trode_data_sink.h"

TrodeDataSink::default_trode();

// Call this on startup, and any time this trode or the
// default trode settings change
TrodeDataSink::assign_from_pb( ArteTrodeOptPb &t_o,
			       ArteTrodeOptPb &d_o,
			       FilterList      filter_list)
{

  // Copy the values
  id                      = t_o.source_trode();
  trode_opt_version       = t_o.t_o_version();
  daq_id                  = t_o.has_daq_id() ? t_o.daq_id() : d_o.daq_id();
  samps_before_trig       = t_o.has_samps_before_trig() ? t_o.samps_before_trig() : d_o.samps_before_trig();
  samps_after_trig        = t_o.has_samps_after_trig() ? t_o.samps_after_trig() : d_o.samps_after_trig();
  refractory_period_samps = t_o.has_refractory_period_samps() ? t_o.refractory_period_samps() : d_o.refractory_period_samps();
  port                    = t_o.has_port() ? t_o.port() : d_o.port();
  host_ip                 = t_o.has_host_ip() ? t_o.host_ip() : d_o.host_ip();
  disk                    = t_o.has_disk() ? t_o.disk() : d_o.disk();
  std::string filt_name   = t_o.has_filter() ? t_o.filter() : d_o.filter ();

  filter = *(filter_list[filt_name]);

  int t_o_n_chans = t_o.chans_size();
  int d_o_n_chans = d_o.chans_size();
  for(int i = 0; i < t_o_n_chans; i++){

      ArteChanOptPb this_chan = t_o.chans(i);
      // if we have enough default chans, use one.  ohterwise, mock out default
      // with real trodeOpt's chan
      ArteChanOptPB this_def = (i < d_o_chans) ? d_o.chans(i) : t_o.chans(i);

      double _gain = this_chan.has_gain() ? this_chan.gain() : this_def.gain();
      input_gain.push_back ( _gain );
      double _th = this_chan.has_threshold() ? this_chan.threshold() : this_def.threshold();
      threshold.push_back  ( _th );
      double _d = this_chan.has_daq_chan() ? this_chan.daq_chan() : this_def.daq_chan();
      daq_chans.push_back  ( _d );
  }

  // Condition the buffers
  data.resize( extents[t_o_n_chans][n_samps] );

}
