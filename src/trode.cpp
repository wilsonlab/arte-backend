// object represents 1 tetrode.  Takes raw data in full buffers, filters the needed channels,
// stores the filtered data, scans it for spikes, sends the spikes to a drawing object, and to
// central file-saving object

#include "trode.h"
#include "util.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

Trode::Trode(){
}


Trode::~Trode(){

}

//std::map <int, neural_daq> neural_daq_map;

int Trode::init(boost::property_tree::ptree &trode_pt, boost::property_tree::ptree &default_pt, 
		std::map<int,neural_daq> &neural_daq_map, std::map<std::string,Filt> &filt_map){

  trode_name = trode_pt.data();
  assign_property<int> ("n_chans", &(trode_opt.n_chans), trode_pt, default_pt, 1);
  trode_opt.thresholds = new double [trode_opt.n_chans];
  assign_property<double>("thresholds", trode_opt.thresholds, trode_pt, default_pt,trode_opt.n_chans);
  trode_opt.channels = new int[trode_opt.n_chans];
  assign_property<int>("channels", trode_opt.channels, trode_pt, default_pt, trode_opt.n_chans);
  assign_property<int>("daq_id", &(trode_opt.daq_id), trode_pt,default_pt, 1);
  assign_property<std::string>("filt_name", &(trode_opt.filt_name), trode_pt, default_pt, 1);
  assign_property<int>("samps_before_trig", &(trode_opt.samps_before_trig), trode_pt, default_pt, 1);
  assign_property<int>("samps_after_trig", &(trode_opt.samps_after_trig), trode_pt, default_pt, 1);
  assign_property<std::string> ("spike_mode", &(trode_opt.spike_mode), trode_pt, default_pt, 1);

  neural_daq my_daq = neural_daq_map.find(trode_opt.daq_id)->second;

  trode_opt.n_samps_per_spike = 1 + trode_opt.samps_before_trig + trode_opt.samps_after_trig;
  trode_opt.buffer_mult_of_input = trode_opt.n_samps_per_spike / my_daq.n_samps_per_buffer;
  if((trode_opt.n_samps_per_spike % my_daq.n_samps_per_buffer) > 0 )
    trode_opt.buffer_mult_of_input += 1;

  if(filt_map.find(trode_opt.filt_name) == filt_map.end()){
    std::cerr << "In Trode::init of trode " << trode_opt.trode_name 
	      << " looked for filt named " << trode_opt.filt_name
	      << " but couldn't find it." << std::endl;
  }
  assert(filt_map.find(trode_opt.filt_name) != filt_map.end());

  trode_opt.my_filt = filt_map.find(trode_opt.filt_name)->second;

  int min_samps_for_filt = trode_opt.my_filt.order;
  trode_opt.my_filt.buffer_mult_of_input = min_samps_for_filt / my_daq.n_samps_per_buffer;
  if( min_samps_for_filt % my_daq.n_samps_per_buffer > 0)
    trode_opt.my_filt.buffer_mult_of_input += 1;

  trode_opt.my_filt.buffer_mult_of_input += trode_opt.my_filt.filtfilt_wait_n_buffers;

  trode_opt.my_filt.n_samps_per_chan = my_daq.n_samps_per_buffer * trode_opt.my_filt.buffer_mult_of_input;

}

void trode_filter_data(Trode *, float64 *, int){

}
