// object represents 1 tetrode.  Takes raw data in full buffers, filters the needed channels,
// stores the filtered data, scans it for spikes, sends the spikes to the tubes, and to
// central file-saving object

#include "trode.h"
#include "util.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <iostream>

int tmp;

Trode::Trode(){
  //std::cout << "In a trode constructor" << std::endl;
}


Trode::~Trode(){
  //std::cout << "In destructor of tetrode object for tetrode: " << trode_name << std::endl; (<- interesting results)
}


int Trode::init(boost::property_tree::ptree &trode_pt, boost::property_tree::ptree &default_pt, 
		std::map<int,neural_daq> &neural_daq_map, std::map<std::string,Filt> &filt_map){

  trode_opt.trode_name = trode_pt.data();
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

  my_daq = &(neural_daq_map[trode_opt.daq_id]);

  trode_opt.stream_n_samps_per_chan = my_daq->n_samps_per_buffer;
  trode_opt.n_samps_per_spike = 1 + trode_opt.samps_before_trig + trode_opt.samps_after_trig;
  trode_opt.buffer_mult_of_input = trode_opt.n_samps_per_spike / my_daq->n_samps_per_buffer;
  if((trode_opt.n_samps_per_spike % my_daq->n_samps_per_buffer) > 0 )
    trode_opt.buffer_mult_of_input += 1;

  if(filt_map.find(trode_opt.filt_name) == filt_map.end()){
    std::cerr << "In Trode::init of trode " << trode_opt.trode_name 
	      << " looked for filt named " << trode_opt.filt_name
	      << " but couldn't find it." << std::endl;
  }
  assert(filt_map.find(trode_opt.filt_name) != filt_map.end());

  trode_opt.my_filt = filt_map.find(trode_opt.filt_name)->second;

  int min_samps_for_filt = trode_opt.my_filt.order;
  trode_opt.my_filt.buffer_mult_of_input = min_samps_for_filt / my_daq->n_samps_per_buffer + 2;
  if( min_samps_for_filt % my_daq->n_samps_per_buffer > 0)
    trode_opt.my_filt.buffer_mult_of_input += 1;

  trode_opt.my_filt.buffer_mult_of_input += trode_opt.my_filt.filtfilt_wait_n_buffers;

  trode_opt.my_filt.n_samps_per_chan = my_daq->n_samps_per_buffer * trode_opt.my_filt.buffer_mult_of_input;
  trode_opt.buf_len = trode_opt.my_filt.n_samps_per_chan;
  trode_opt.buf_size_bytes = trode_opt.my_filt.n_samps_per_chan * trode_opt.n_chans * sizeof(u_buf[0]);
  trode_opt.my_filt.out_buf_size_bytes = trode_opt.buf_size_bytes;
  filt_map[trode_opt.filt_name] = trode_opt.my_filt;
  //pp_to_raw_stream = &(my_daq->copy_flexptr);
  ptr_to_raw_stream = my_daq->data_ptr;

  //  u_buf = new float64 [trode_opt.my_filt.n_samps_per_chan * trode_opt.n_chans];
  //f_buf = new float64 [trode_opt.my_filt.n_samps_per_chan * trode_opt.n_chans];
  //ff_buf = new float64 [trode_opt.my_filt.n_samps_per_chan * trode_opt.n_chans];

  u_curs = 0;
  f_curs = 0;
  ff_curs = 0;
  tmp = 0;
}

//void Trode:: process_data(){
//  
//}

void *trode_filter_data(void *t){
  Trode *trode = (Trode *)t;
  //  std::cout << "About to call filter_data from the trode." << std::endl;
  if(tmp == 0){
    //std::cout << "About to call filter_data from the trode: " << trode->trode_opt.trode_name << std::endl;
    //fflush(stdout);
    filter_data(trode->ptr_to_raw_stream, &(trode->trode_opt.my_filt), trode->my_daq, trode->trode_opt.channels, 
		trode->trode_opt.n_chans, trode->my_daq->n_samps_per_buffer, 
		trode->trode_opt.buf_len, &(trode->u_curs), &(trode->f_curs), 
		&(trode->ff_curs),trode->u_buf, trode->f_buf, trode->ff_buf);
    //tmp = 1;
    //print_array(trode->u_buf, trode->trode_opt.n_chans, trode->trode_opt.my_filt.n_samps_per_chan, trode->u_curs);  
  } else{
    //std::cout << "skip" << std::endl;
    //fflush(stdout);
  }
}


void Trode::print_options(void){
  std::cout << "********Trode: " << trode_opt.trode_name << "********" << std::endl
	    << "n_chans:      " << trode_opt.n_chans << std::endl
	    << "channels: ";
  for (int a = 0; a < trode_opt.n_chans; a++)
    std::cout << trode_opt.channels[a] << " ";
  std::cout << std::endl
	    << "thresholds:   ";
  for(int a = 0; a < trode_opt.n_chans; a++)
    std::cout << trode_opt.thresholds[a] << " ";
  std:: cout << std::endl
	     << "daq_id:      " << trode_opt.daq_id << std::endl
	     << "filt_name:   " << trode_opt.filt_name << std::endl
	     << "filt.order:  " << trode_opt.my_filt.order << std::endl
	     << "spike_nsamps:" << trode_opt.n_samps_per_spike << std::endl << std::endl;
}


void Trode::print_buffers(int chan_lim, int samp_lim){
  system("clear");
  std::cout << "*********" << trode_opt.trode_name << "*************" << std::endl;
  std::cout << std::fixed << std::setprecision(1); 
  neural_daq this_daq = neural_daq_map[trode_opt.daq_id];
  std::cout << "raw_stream || u_buf  ||  f_buf" << std::endl;

  for (int s = 0; s < samp_lim; s++){

    int neural_daq_row_offset = s * my_daq->n_chans;
    int row_offset = s * trode_opt.n_chans;

    for(int c = 0; c < chan_lim; c++){
      int this_in_c = trode_opt.channels[c];
      //      std::cout << std::setw(7) << ptr_to_raw_stream[this_in_c + neural_daq_row_offset] << " "; // correct output
      std::cout << std::setw(7) << my_daq->data_ptr[this_in_c + neural_daq_row_offset] << " "; // correct output
      //       std::cout << std::setw(7) << my_daq->data_ptr_copy[this_in_c + neural_daq_row_offset] << " ";  // all 8.0's
      //      std::cout << std::setw(7) << this_daq.data_ptr_copy[this_in_c + neural_daq_row_offset] << " ";  // all 8.0's
    }

    std::cout << "  ||  ";

    for (int c = 0; c < chan_lim; c++){
      if(s == u_curs){
	std::cout << "\033[0;32m";
      }else{
	std::cout << "\033[0m";}
      std::cout << std::setw(7) << u_buf[c + row_offset] << " \033[0m"; 
    }

    std::cout << "  ||  ";

    std::cout << std::fixed << std::setprecision(1);
    for (int c = 0; c < chan_lim; c++){
      if(s == f_curs){
	std::cout << "\033[0;32m";
      }else{
	std::cout << "\033[0m";}
      std::cout << std::setw(7) << u_buf[c + row_offset + (trode_opt.n_chans * trode_opt.buf_len)] << " \033[0m";
    }

    std::cout << "  ||  ";
  
    std::cout << std::fixed << std::setprecision(1);
    for (int c = 0; c < chan_lim; c++){
      if(s == f_curs){
	std::cout << "\033[0;32m";
      } else{
	std::cout << "\033[0m";}
      std::cout << std::setw(7) << f_buf[c + row_offset] << "  \033[0m";
    }


    std::cout <<std::endl;

  }

  //fflush(stdout);
  //printf("\b \r \b \r \b \r \b \r");
  //system("clear");
  //exit(1);
}


void Trode::print_spikes(void){

}
