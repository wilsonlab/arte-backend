#include "spike.h"

Spike::Spike(uint16_t name_, int n_chans_, int n_samps_, 
      int samp_n_bytes_, int16_t gains_[], 
      rdata_t thresh_[], uint16_t trig_ind_ ){

  // Initialize vars
  name = name_;
  n_chans = n_chans_;
  n_samps_per_chan = n_samps_;
  samp_n_bytes = samp_n_bytes_;
  trig_ind = trig_ind_;
  for (int s = 0; s < n_samps_per_chan; s++){
    gains[s] = gains_[s];
    thresh[s] = thresh_[s];
  }

  for (int c = 0; c < n_chans; c++){
    for(int s = 0; s < n_samps_per_chan; s++){
      // 'unset' value
      data[c*n_samps_per_chan + s] = -2.0;
    }
  }

  // Initialize protobuf
  my_spike = my_pb.mutable_arte_spike();
  for(int c = 0; c < n_chans; c++){
    ArteVoltageTimeseries *v = my_spike->add_chan_data();
    for(int s = 0; s < n_samps_per_chan; s++){
      // 'unset' value
      v->add_voltage( -5.5 );
    }
  }

  // Initialize spike_net_t
  my_spike_net.ts = 8.0;
  my_spike_net.name = name_;
  my_spike_net.n_chans = n_chans_;
  my_spike_net.n_samps_per_chan= n_samps_;
  my_spike_net.samp_n_bytes = samp_n_bytes_;
  my_spike_net.trig_ind = trig_ind_;
  for (int s = 0; s < n_chans; s++){
    my_spike_net.gains[s] = gains_[s];
    my_spike_net.thresh[s] = thresh_[s];
    for(int c = 0; c < n_samps_per_chan; c++){
      my_spike_net.data[c*n_samps_per_chan + s] = -16.0;
    }
  }
  
}

ArtePb & Spike::get_pb() {
  my_pb.set_timestamp ( ts );
  my_spike->set_source_trode ( name );
  for ( int c = 0; c < n_chans; c++ ){

    ArteVoltageTimeseries *this_chan = my_spike->mutable_chan_data(c);
    for(int s = 0; s < n_samps_per_chan; s++)
      this_chan->set_voltage( s, data[c*n_samps_per_chan + s] );

  }
}

void Spike::get_spike_net(spike_net_t *sn) {
  sn->ts = ts;
  sn->name = name;
  sn->n_chans = n_chans;
  sn->n_samps_per_chan = n_samps_per_chan;
  sn->samp_n_bytes = samp_n_bytes;
  sn->trig_ind = trig_ind;
  sn->seq_num = seq_num;
  for (int s = 0; s < n_samps_per_chan; s++){
    sn->gains[s] = gains[s];
    sn->thresh[s] = thresh[s];
    for (int c = 0; c < n_chans; c++)
      sn->data[c*n_samps_per_chan + s] = data [c*n_samps_per_chan + s];
  }
}
