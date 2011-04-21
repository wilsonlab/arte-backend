#include "lfp_bank.h"
#include <iostream>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <assert.h>

Lfp_bank::Lfp_bank(){
  std::cout << "Lfp_bank constructor called." << std::endl;
}

Lfp_bank::~Lfp_bank(){
  std::cout << "Lfp_bank destructor called." << std::endl;
  print_options();
}

int Lfp_bank::init(boost::property_tree::ptree &lfp_bank_pt,
		   std::map<int, neural_daq> &neural_daq_map,
		   std::map<std::string, Filt> &filt_map){
  
  std::cout << "Beginning lfp_bank assign_property block." << std::endl;
  
  std::istringstream iss (lfp_bank_pt.data());

  iss >> lfp_bank_name;
  assign_property<uint16_t>("n_chans", &n_chans, lfp_bank_pt, lfp_bank_pt, 1);
  assign_property<uint16_t>("daq_id",  &daq_id,  lfp_bank_pt, lfp_bank_pt, 1);
  assign_property<uint16_t>("channels",channels, lfp_bank_pt, lfp_bank_pt, n_chans);
  assign_property<std::string>("filt_name", &filt_name, lfp_bank_pt, lfp_bank_pt, 1);
  assign_property<uint16_t>("keep_nth_sample", &keep_nth_sample, lfp_bank_pt, lfp_bank_pt, 1);
  assign_property<std::string>("buffer_dump_filename", &buffer_dump_filename, lfp_bank_pt, lfp_bank_pt, 1);
  assign_property<std::string>("lfp_bank_dump_filename",&lfp_bank_dump_filename,lfp_bank_pt,lfp_bank_pt,1);

  std::cout << "Finished assign_property block." << std::endl;

  my_daq = &(neural_daq_map[daq_id]);
  assert(filt_map.find(filt_name) != filt_map.end()); // make sure named filt exists
  my_filt = filt_map[filt_name];

  stream_n_samps_per_chan = my_daq->n_samps_per_buffer;
  stream_n_chans = my_daq->n_chans;

  int min_samps_for_filt = my_filt.order;
  my_filt.buffer_mult_of_input = min_samps_for_filt / my_daq->n_samps_per_buffer + 2;

  // usually filt needs 4 to 8 samps.  4 / about 32 samps per buffer = 0. Add 2 buffers
  // to be safe.  (having a big circular buffer probably doesn't hurt)
  my_filt.buffer_mult_of_input += my_filt.filtfilt_wait_n_buffers;
  my_filt.n_samps_per_chan = my_filt.buffer_mult_of_input * stream_n_samps_per_chan;

  // if stream_n_samps_per_chan % keep_nth_samp is > 0, then the output
  // buffers will 'gallop'.  So reduce the downsampling until we reach
  // a non-galloping interval.  (should we do it this way, or throw error instead?)
  while( (stream_n_samps_per_chan % keep_nth_sample) > 0 ){
    printf("\a");
    std::cout << "(stream_n_samps_per_chan=" << stream_n_samps_per_chan <<
      ") % (keep_nth_sample=" << keep_nth_sample << ") = " <<
      stream_n_samps_per_chan % keep_nth_sample << ".  Decrimenting by 1 to " <<
      (keep_nth_sample - 1) << std::endl;
    keep_nth_sample--;
  }
  
  buf_len = stream_n_samps_per_chan / keep_nth_sample;

  ptr_to_raw_stream = &(my_daq->data_ptr);

  u_curs = 0;
  f_curs = 0;
  ff_curs = 0;

  if(!buffer_dump_filename.empty()){
    buffer_dump_file = try_fopen( buffer_dump_filename.c_str(), "wb" );
    uint32_t tmp = 0;
    try_fwrite<uint32_t>( &tmp, 1, buffer_dump_file );  // placeholder for buffer count
    try_fwrite<uint16_t>( &(stream_n_chans), 1, buffer_dump_file );
    try_fwrite<uint16_t>( &(stream_n_samps_per_chan), 1, buffer_dump_file );
  }

  if(!lfp_bank_dump_filename.empty()){
    lfp_bank_dump_file = try_fopen( lfp_bank_dump_filename.c_str(), "wb" );
    uint32_t tmp = 0;
    try_fwrite<uint32_t>( &tmp,     1, lfp_bank_dump_file ); // placeholder for buffer count
    try_fwrite<uint16_t>( &n_chans, 1, lfp_bank_dump_file);
    try_fwrite<uint16_t>( &buf_len, 1, lfp_bank_dump_file);
  }
  
}

void Lfp_bank::print_options(void){
  std::cout << "still got to implement Lfp_bank::print_options()" << std::endl;
}

void *lfp_bank_filter_data(void *lfp_bank_in){    // seems this should now be called 'lfp_bank_process_buffer' b/c it does multiple things

  //std::cout << "In lfp_bank_filter_data\n";
  Lfp_bank* this_bank = (Lfp_bank*) lfp_bank_in;
  // call filter_data from filt.h  We probably should modify this function declaration b/c I think many of the arguments passed can be figured out by the fn itself)
  // if we can just pass filter a reference to the lfp_bank object.  (can filt.cpp include lfp_bank.h I wonder?)
  filter_data(*(this_bank->ptr_to_raw_stream), &(this_bank->my_filt), this_bank->my_daq, this_bank->channels,
	      this_bank->n_chans, 
	      this_bank->my_daq->n_samps_per_buffer,  // n_daq_samps_per_buffer_per_channel
	      this_bank->my_filt.n_samps_per_chan,    // filt buff length (before downsampling)
	      &(this_bank->u_curs),
	      &(this_bank->f_curs),
	      &(this_bank->ff_curs), 
	      this_bank->u_buf, this_bank->f_buf, this_bank->ff_buf);

  // Now copy from the filtered buffer into the downsampled buffer
  for(int i = 0; i < this_bank->buf_len; i++){
    for(int c = 0; c < this_bank->n_chans; c++){
      int samp_ind = i * this_bank->keep_nth_sample;
      this_bank->d_buf[ (i*this_bank->n_chans) + c ] = this_bank->f_buf[ (samp_ind*this_bank->n_chans) + c ];
    }
  }

  lfp_bank_write_record(this_bank);

}

void lfp_bank_write_record(void *lfp_bank_in){
  //std::cout << "in write_record\n";
  Lfp_bank* this_bank = (Lfp_bank*) lfp_bank_in;
  uint16_t recordSizeBytes = 0;
  //std::cout << std::setw(6);
  if(this_bank->my_daq->buffer_timestamp  % (10 * 250) == 0){
    //std::cout << this_bank->my_daq->buffer_timestamp / 10000.0 << " ";
    for(int s = 0; s < this_bank->buf_len; s++){
      std::cout << this_bank->my_daq->buffer_timestamp / 10000 << " ";
      for(int c = 0; c < this_bank->n_chans; c++){
	std::cout << this_bank->d_buf[s * this_bank->n_chans + c] << " ";
      }
      std::cout << std::endl;
    }
  }
}

