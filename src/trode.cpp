// object represents 1 tetrode.  Takes raw data in full buffers, filters the needed channels,
// stores the filtered data, scans it for spikes, sends the spikes to the tubes, and to
// central file-saving object

#include "trode.h"
#include "util.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <iostream>
#include <stdint.h>
#include "spike_detector.h"
#include "filter_fun.h"

int tmp;

Trode::Trode(){
  has_sockfd = false;
  //std::cout << "In a trode constructor" << std::endl;
}


Trode::~Trode(){
  if (has_sockfd)
    close(my_netcomdat.sockfd);
  //printf("caught an attempt to close sockfd from a trode.\n");
    //std::cout << "In destructor of tetrode object for tetrode: " << trode_name << std::endl; (<- interesting results)
}

// Old init function using std::maps for trode_map, neural_daq_map, filt_map


int Trode::init(boost::property_tree::ptree &trode_pt, 
		boost::property_tree::ptree &default_pt, 
		std::map<int,neural_daq> &neural_daq_map, 
		std::map<std::string,Filt> &filt_map){
  

//   std::cout << "Beginning of assign_property block." << std::endl;

//   std::istringstream iss(trode_pt.data()); // initialize istringstream with trode_pt.data() which is std::string trode name
//   iss >> trode_opt.trode_name;
//   //trode_opt.trode_name = trode_pt.data();
  n_chans = 1;
  

//   assign_property<uint16_t> ("n_chans", &(trode_opt.n_chans), trode_pt, default_pt, 1);
//   assign_property_ftor<rdata_t>("thresholds", trode_opt.thresholds, trode_pt, default_pt,trode_opt.n_chans);
//   assign_property<uint16_t>("channels", trode_opt.channels, trode_pt, default_pt, trode_opt.n_chans);
//   assign_property<uint16_t>("daq_id", &(trode_opt.daq_id), trode_pt,default_pt, 1);
//   assign_property<name_string_t>("filt_name", &(trode_opt.filt_name), trode_pt, default_pt, 1);
//   assign_property<uint16_t>("samps_before_trig", &(trode_opt.samps_before_trig), trode_pt, default_pt, 1);
//   assign_property<uint16_t>("samps_after_trig", &(trode_opt.samps_after_trig), trode_pt, default_pt, 1);
//   assign_property<name_string_t> ("spike_mode", &(trode_opt.spike_mode), trode_pt, default_pt, 1);
//   assign_property<name_string_t> ("buffer_dump_filename", &(trode_opt.buffer_dump_filename), trode_pt, default_pt, 1);

//   std::cout << "Finished assign_property block." << std::endl;

//   my_daq = &(neural_daq_map[trode_opt.daq_id]);

//   trode_opt.stream_n_samps_per_chan = my_daq->n_samps_per_buffer;
//   trode_opt.n_samps_per_spike = 1 + trode_opt.samps_before_trig + trode_opt.samps_after_trig;
//   trode_opt.buffer_mult_of_input = trode_opt.n_samps_per_spike / my_daq->n_samps_per_buffer;
//   if((trode_opt.n_samps_per_spike % my_daq->n_samps_per_buffer) > 0 )
//     trode_opt.buffer_mult_of_input += 1;

//   if(filt_map.find(trode_opt.filt_name) == filt_map.end()){
//     std::cerr << "In Trode::init of trode " << trode_opt.trode_name 
// 	      << " looked for filt named " << trode_opt.filt_name
// 	      << " but couldn't find it." << std::endl;
//   }
//   assert(filt_map.find(trode_opt.filt_name) != filt_map.end());

//   trode_opt.my_filt = filt_map.find(trode_opt.filt_name)->second;

//   int min_samps_for_filt = trode_opt.my_filt.order;
//   trode_opt.my_filt.buffer_mult_of_input = min_samps_for_filt / my_daq->n_samps_per_buffer + 2;
//   if( min_samps_for_filt % my_daq->n_samps_per_buffer > 0)
//     trode_opt.my_filt.buffer_mult_of_input += 1;

//   trode_opt.my_filt.buffer_mult_of_input += trode_opt.my_filt.filtfilt_wait_n_buffers;

//   trode_opt.my_filt.n_samps_per_chan = my_daq->n_samps_per_buffer * trode_opt.my_filt.buffer_mult_of_input;
//   trode_opt.buf_len = trode_opt.my_filt.n_samps_per_chan;
//   trode_opt.buf_size_bytes = trode_opt.my_filt.n_samps_per_chan * trode_opt.n_chans * sizeof(u_buf[0]);
//   trode_opt.my_filt.out_buf_size_bytes = trode_opt.buf_size_bytes;

//   // WHY DID I DO THIS?  I believe filts from map are copied into each trode, not
//   // referenced from each trode.  This is important b/c filts acquire some
//   // trode-spacefic values (and lfp specific values, in lfp_banks).
//   filt_map[trode_opt.filt_name] = trode_opt.my_filt;

//   ptr_to_raw_stream = &(my_daq->data_ptr);

//   if(!trode_opt.buffer_dump_filename.empty()){
//     buffer_dump_file = try_fopen( trode_opt.buffer_dump_filename.c_str(), "wb" );
//     uint32_t tmp = 0;
//     try_fwrite<uint32_t>( &tmp, 1, buffer_dump_file );
//     try_fwrite<uint16_t>( &(trode_opt.n_chans), 1, buffer_dump_file );
//     try_fwrite<uint16_t>( &(my_daq->n_samps_per_buffer), 1, buffer_dump_file );
//   }

//   u_curs = 0;
//   f_curs = 0;
//   ff_curs = 0;
//   tmp = 0;

//   print_options();

}

// New init function for the global array situation
void Trode::init2(boost::property_tree::ptree &trode_pt,
		  boost::property_tree::ptree &default_pt,
		  Filtered_buffer * fb_curs){

  std::string tmp_spikemode;
  // assign trode-specific properties
  assign_property<uint16_t>("n_chans", &n_chans, trode_pt, default_pt, 1);
  assign_property_ftor<rdata_t>("thresholds",  thresholds, trode_pt, default_pt, n_chans);
  assign_property<uint16_t>("samps_before_trig", &samps_before_trig, trode_pt, default_pt, 1);
  assign_property<uint16_t>("samps_after_trig" , &samps_after_trig , trode_pt, default_pt, 1);
  assign_property<uint16_t>("refractory_period_samps",&refractory_period_samps,trode_pt,default_pt,1);
  assign_property<std::string>("spike_mode"    , &tmp_spikemode    , trode_pt, default_pt, 1);
  strcpy( spike_mode, tmp_spikemode.c_str() );

  n_samps_per_spike = samps_before_trig + samps_after_trig + 1;
  my_buffer = fb_curs;

  // Filtered_buffer::init() should take 2 property trees and a min sample number
  // We need at very least n_samps_before + n_samps_after + 1 for the trig sample + 1 for padding
  // NB check the reasoning for the above count.  Do we need a longer buffer?
  my_buffer->init(trode_pt, default_pt, (n_samps_per_spike + 1) );

  // Initialize the netcom
  //extern std::vector<NetCom> netcom_vector;

  my_netcom = new NetCom;
  std::string net_on;
  std::string host_ip;
  int port_num;
  char host_str[INET6_ADDRSTRLEN], port_str[INET6_ADDRSTRLEN];  // maybe used wrong const size here? 
  assign_property<std::string>("host_ip", &host_ip,   trode_pt, default_pt, 1);
  assign_property<int>("port",    &port_num, trode_pt, default_pt, 1);
  assign_property<std::string>("network", &net_on,    trode_pt, default_pt, 1);
  strcpy(host_str, host_ip.c_str());
  //strcpy(port_str, port_name.c_str());
  //netcom_vector.push_back(my_netcom);
  //delete my_netcom;
  //my_netcom = &(netcom_vector.back());
  if( strcmp( "on", net_on.c_str())==0 ){
    my_netcom->initUdpTx( host_str, port_num );
    printf("Successfully connected tetrode %d to host_ip %s at port %d\n",
	   name, host_ip.c_str(), port_num);
    has_sockfd = true;
  }

}

void *trode_filter_data(void *t){

//   **********OLD WAY******
//   Trode *trode = (Trode *)t;
//   //  std::cout << "About to call filter_data from the trode." << std::endl;
//   if(tmp == 0){
//     //std::cout << "About to call filter_data from the trode: " << trode->trode_opt.trode_name << std::endl;
//     //fflush(stdout);
//     filter_data(*(trode->ptr_to_raw_stream), &(trode->trode_opt.my_filt), trode->my_daq, trode->trode_opt.channels, 
// 		trode->trode_opt.n_chans, trode->my_daq->n_samps_per_buffer, 
// 		trode->trode_opt.buf_len, &(trode->u_curs), &(trode->f_curs), 
// 		&(trode->ff_curs),trode->u_buf, trode->f_buf, trode->ff_buf);

//     find_spikes(trode);
//     //tmp = 1;
//     //print_array(trode->u_buf, trode->trode_opt.n_chans, trode->trode_opt.my_filt.n_samps_per_chan, trode->u_curs);  
//   } else{
//     //std::cout << "skip" << std::endl;
//     //fflush(stdout);
//   }

  filter_buffer( ((Trode *)t)->my_buffer );
  

}


void Trode::print_options(void){
  std::cout << "********Trode: " << name << "********" << std::endl
	    << "n_chans:      " << n_chans << std::endl
	    << "channels: ";
  for (int a = 0; a < n_chans; a++)
    std::cout << my_buffer->channels[a] << " ";
  std::cout << std::endl
	    << "thresholds:   ";
  for(int a = 0; a < n_chans; a++)
    std::cout << thresholds[a] << " ";
  std:: cout << std::endl
	     << "daq_id:      " << my_buffer->daq_id << std::endl
	     << "filt_name:   " << (my_buffer->my_filt).filt_name << std::endl
	     << "filt.order:  " << (my_buffer->my_filt).order << std::endl
	     << "spike_nsamps:" << n_samps_per_spike << std::endl << std::endl;
}


void Trode::print_buffers(int chan_lim, int samp_lim){


//   system("clear");
//   std::cout << "*********" << name << "*************" << std::endl;
//   std::cout << std::fixed << std::setprecision(1); 
//   neural_daq this_daq = neural_daq_map[my_buffer->my_daq->id];
//   std::cout << "raw_stream || u_buf  ||  f_buf" << std::endl;

//   for (int s = 0; s < samp_lim; s++){

//     int neural_daq_row_offset = s * (my_buffer->my_daq)->n_chans;
//     int row_offset = s * n_chans;
//     int last_curs = CBUF( (my_buffer->u_curs - (my_buffer->my_daq)->n_samps_per_buffer), my_buffer->buf_len);
//     int adjusted_u_curs;
//     if(my_buffer->u_curs == 0){
//       adjusted_u_curs = my_buffer->buf_len;
//     }
//     else{
//       adjusted_u_curs = my_buffer->u_curs;
//     }

//     if( s < last_curs || s >= adjusted_u_curs){
//       for(int c = 0; c < chan_lim; c++){
// 	std::cout << "\033[0;32m" << std::setw(7) << 0.0 << " " << "\033[0m";
//       }
//     }
//     else{
//       for(int c = 0; c < chan_lim; c++){
// 	int this_in_c = (my_buffer->channels)[c];
// 	//      std::cout << std::setw(7) << ptr_to_raw_stream[this_in_c + neural_daq_row_offset] << " "; // correct output
// 	std::cout << std::setw(7) << (my_buffer->my_daq)->data_ptr[(this_in_c + neural_daq_row_offset) - (last_curs * (my_buffer->my_daq)->n_chans)] << " "; // correct output
// 	//       std::cout << std::setw(7) << my_daq->data_ptr_copy[this_in_c + neural_daq_row_offset] << " ";  // all 8.0's
// 	//      std::cout << std::setw(7) << this_daq.data_ptr_copy[this_in_c + neural_daq_row_offset] << " ";  // all 8.0's
//       }
//     }

//     std::cout << "  ||  ";

//     for (int c = 0; c < chan_lim; c++){
//       if(s == (my_buffer->u_curs)){
// 	std::cout << "\033[0;32m";
//       }else{
// 	std::cout << "\033[0m";}
//       std::cout << std::setw(7) << my_buffer->u_buf[c + row_offset] << " \033[0m"; 
//     }

// //     std::cout << "  ||  ";

// //     std::cout << std::fixed << std::setprecision(1);
// //     for (int c = 0; c < chan_lim; c++){
// //       if(s == f_curs){
// // 	std::cout << "\033[0;32m";
// //       }else{
// // 	std::cout << "\033[0m";}
// //       std::cout << std::setw(7) << u_buf[c + row_offset + (trode_opt.n_chans * trode_opt.buf_len)] << " \033[0m";
// //     }


//     std::cout << "  ||  ";
//     std::cout << std::fixed << std::setprecision(1);
//     for (int c = 0; c < chan_lim; c++){
//       if(s == my_buffer->f_curs){
// 	std::cout << "\033[0;32m";
//       } else{
// 	std::cout << "\033[0m";}
//       std::cout << std::setw(7) << my_buffer->f_buf[c + row_offset] << "  \033[0m";
//     }
//     std::cout <<std::endl;
//   }


}


void Trode::print_spikes(void){

}
