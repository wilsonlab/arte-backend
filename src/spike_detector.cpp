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
