#include "trode_data_sink.h"
#include "filt.h"

void TrodeDataSink::operator()(){
  filter();
  
}

// Call this on startup, and any time this trode or the
// default trode settings change
TrodeDataSink::TrodeDataSink( ArteTrodeOptPb &t_o,
			      ArteTrodeOptPb &d_o,
			      Filt::FiltList &filter_list)
{

  // Copy the values
  id                      = t_o.source_trode();
  trode_opt_version       = t_o.trode_opt_version();
  daq_id                  = t_o.has_daq_id() ? t_o.daq_id() : d_o.daq_id();

  samps_before_trig       = 
    t_o.has_samps_before_trig() ? t_o.samps_before_trig() : d_o.samps_before_trig();

  samps_after_trig        = 
    t_o.has_samps_after_trig() ? t_o.samps_after_trig() : d_o.samps_after_trig();

  refractory_period_samps = 
    t_o.has_refractory_period_samps() ? 
    t_o.refractory_period_samps() : d_o.refractory_period_samps();

  port                    = t_o.has_port() ? t_o.port() : d_o.port();
  host_ip                 = t_o.has_host_ip() ? t_o.host_ip() : d_o.host_ip();
  disk                    = t_o.has_disk() ? t_o.disk() : d_o.disk();
  std::string filt_name   = t_o.has_filter() ? t_o.filter().filter_name() : d_o.filter().filter_name();

  int t_o_n_chans = t_o.chans_size();
  int d_o_n_chans = d_o.chans_size();
  for(int i = 0; i < t_o_n_chans; i++){

      ArteChanOptPb this_chan = t_o.chans(i);
      // if we have enough default chans, use one.  ohterwise, mock out default
      // with real trodeOpt's chan
      ArteChanOptPb this_def = (i < d_o_n_chans) ? d_o.chans(i) : t_o.chans(i);

      double _gain = this_chan.has_gain() ? this_chan.gain() : this_def.gain();
      input_gain.push_back ( _gain );
      double _th = this_chan.has_threshold() ? this_chan.threshold() : this_def.threshold();
      threshold.push_back  ( _th );
      double _d = this_chan.has_daq_chan() ? this_chan.daq_chan() : this_def.daq_chan();
      daq_chans.push_back  ( _d );
  }
  n_chans = t_o_n_chans;
  n_samps = samps_before_trig + 1 + samps_after_trig;

  // Condition the buffers
  data.request_size( n_chans, n_samps );
  //  data.resize( extents[t_o_n_chans][n_samps] );

  // COPY the filter
  filter = filter_list[filt_name];
  filter.attach_buffers ( &data.voltage_buffer, &filtered_data.voltage_buffer );

  // Initialize the Spike protobuf
  spike_pb = parent_pb.mutable_arte_spike();
  spike_pb->Clear();
  spike_pb->set_source_trode( id );
  for( int c = 0; c < n_chans; c++ ){
    ArteVoltageTimeseries *v = spike_pb->add_chan_data();
    for (int s = 0; s < n_samps; s++){
      v->add_voltage( -4.0 );
    }
  }

  // Initialize spike seeking range
  const raw_voltage_array::size_type *st = my_data_source->data.voltage_buffer.shape();
  //  int source_n_samps = my_data_source->voltage_buffer[0].size();
  int source_n_samps = st[0];
  int store_n_samps  = data.voltage_buffer[0].size();
  spike_seek_end   = store_n_samps - samps_after_trig - filter.filtfilt_invalid_samps;
  spike_seek_start = spike_seek_end - source_n_samps + 1;
}


// Should NeuralVoltageCircBuffer know how to read from a NeuralVoltageBuffer?
// Or should we do this for it, like here?
// (alternatively, maybe sink_data << source_data kind of thing?)
void TrodeDataSink::get_data_from_source(){
  for (int c = 0; c < n_chans; c++ ){
    for (int s = 0; s < n_samps; s++){
      rdata_t this_datum = my_data_source->data.voltage_buffer[c][s];
      data.voltage_buffer[c].push_back( this_datum );
    }
  }
}


void TrodeDataSink::scan_for_spikes(){

  spike_index.clear();
  for(int s = spike_seek_start; s <= spike_seek_end; s++){
    bool is_crossing = false;
    for(int c = 0; c < n_chans; c++){
      if ( filtered_data.voltage_buffer[c][s] >= threshold[c] ){
	is_crossing = true;
      }
    }

    if( is_crossing ){
      spike_index.push_back(s);
      s += refractory_period_samps;
    }
    
  }  // next sample
  
}

void TrodeDataSink::publish_spikes(){

  for (int n = 0; n < spike_index.size(); n++){
    timestamp_t this_ts = filtered_data.ts_of_index( spike_index[n], 
						     my_data_source->ts_ticks_per_samp );
    parent_pb.set_timestamp( this_ts );
    for (int c = 0; c < n_chans; c++){
      ArteVoltageTimeseries *ts = spike_pb->mutable_chan_data(c);
      for(int s = 0; s < n_samps; s++){
	ts->set_voltage( s,  filtered_data.voltage_buffer[c][s] );
      }
    }

    if (disk)
      main_file->pb_write( parent_pb );

    if (network)
      data_netcom->txArtePb( parent_pb );

  }

}

    
