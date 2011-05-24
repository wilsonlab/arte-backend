#include "spike_detector.h"

SpikeDetector::SpikeDetector(){

  // 'unset' values, to make sure init() is only called after these have been set
  n_samps_before_spike = -1;
  n_samps_after_spike = -1;
  n_chans = -1;
  //n_samps_per_chan = -1;

}

SpikeDetector::~SpikeDetector(){
  std::cout << "In SpikeDetector destructor." << std::endl;
}


void SpikeDetector::init(Filtered_buffer *in_fb){
  
  if(n_samps_before_spike == -1 ||
     n_samps_after_spike == -1  ){
    std::cout << "SpikeDetector::init() called but object isn't ready to init." << std::endl;
  }
  
  fb = in_fb;
  
  n_chans = fb->n_chans;
  // TEMP commented out...
  //buffer_n_samps_per_chan = fb->n_samps_per_chan;
  spike_n_samps_per_chan = 1 + n_samps_before_spike + n_samps_after_spike;
  
  thresh =       new rdata_t [n_chans];
  search_inds =  new int [spike_n_samps_per_chan];
  //spike_inds =   new ind[spike_n_samps_per_chan];
  //spike_buffer = new rdata_t [n_chans * spike_n_samps_per_chan];

  

}

void find_spikes(Trode *trode, SpikeDetector *sd){

  int buf_len, n_chans, search_start_ind, search_stop_ind, stop_ind; 
  int samps_before_trig, n_samps_per_spike, search_cursor, s, c;
  bool found_spike;
  rdata_t *thresh, *spike_buffer, *f_buf;
  Filtered_buffer *fb;
  
  buf_len = fb->buf_len;

  search_stop_ind  = CBUF( (fb->f_curs + fb->stream_n_samps_per_chan - trode->samps_after_trig),
		    buf_len);
  search_start_ind = CBUF( (stop_ind - trode->n_samps_per_spike),
		    buf_len);

  for(search_cursor = search_start_ind; 
      search_cursor != search_stop_ind; 
      search_cursor = CBUF(++search_cursor, buf_len)){
    
    found_spike = 0;
    for(c = 0; c < n_chans; c++){
      found_spike += (f_buf[ search_cursor * n_chans + c] > thresh[c]);  // good idea to treat bool like int?
    }
    
    if(found_spike > 0){
      for(s = 0; s < n_samps_per_spike; s++){
	for(c = 0; c < n_chans; c++){
	  spike_buffer[ s * n_chans + c ] = f_buf[ CBUF((search_cursor-samps_before_trig+s), buf_len)*n_chans + c];
	}
      }

      search_cursor += trode->refractory_period_samps - 1; // drop 1 1b/c we add the 1 in the for loop

      //spike_to_disk(spike_buffer, n_chans, n_samps_per_chan, trode);
      //spike_to_net (spike_buffer, n_chans, n_samps_per_chan, trode);
    }
    
  }
}
