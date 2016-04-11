#include "filtered_buffer.h"
#include "filter_fun.h"
#include "util.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <iostream>
#include <stdint.h>

Filtered_buffer::Filtered_buffer(){
  printf("In a filtered_buffer constructor.\n");
  fflush(stdout);
}

Filtered_buffer::~Filtered_buffer(){
  
  std::cout << "Filtered_buffer destructor called on filtered_buffer name: " <<
    name << std::endl;
  
  if( ! finalize_done ){
    finalize_files();
    std::cout << "Filtered_buffer files were not finalized manually. " <<
      "Destructor caught it and called finalize_files() for " <<
      buffer_dump_filename << std::endl;
  }

}

void Filtered_buffer::init(boost::property_tree::ptree &pt,
			   boost::property_tree::ptree &default_pt,
			   int min_samps){
 
  // assign values from the property tree into filtered buffer members
  std::string tmp_filt_name; // no longer a member. Temp var
  std::string tmp_buffer_filename;
  assign_property<uint16_t>("n_chans",                &n_chans, pt, default_pt, 1);
  assign_property<uint16_t>("channels",               channels, pt, default_pt, n_chans);
  assign_property<uint16_t>("daq_id",                 &daq_id,  pt, default_pt, 1);
  assign_property<std::string>("filt_name",           &tmp_filt_name,pt, default_pt, 1);
  assign_property<std::string>("buffer_dump_filename",&tmp_buffer_filename,pt,default_pt,1);
  strcpy( buffer_dump_filename, tmp_buffer_filename.c_str() );
  
  // which neural_daq in the array has the id we want?
  for (int i = 0; i < n_neural_daqs; i++){
    if( daq_id == neural_daq_array[i].id ) {
      my_daq = &(neural_daq_array[i]);
	std::cout << "I USE THIS FOR LOOP WOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO" << std::endl;
 	std::cout << "daq_id: " << daq_id << " i is " << i << " neural_daq_array[i].id is " << neural_daq_array[i].id << std::endl;
	std::cout << daq_id << " == " << neural_daq_array[i].id << " : " << (daq_id == neural_daq_array[i].id) << std::endl;
	 }
 }
  // stream_n_samps_per_chan is the number of samps per chan
  // collected during each daq card read
  stream_n_samps_per_chan = my_daq->n_samps_per_buffer;
  stream_n_chans          = my_daq->n_chans; 

  // min_samps, passed by the caller, is the minimum number
  // of samples needed in the (circular) filtered buffer
  // trodes need a filtered buffer at least the length of a spike
  // lfp_bank buffer length tends to be determined by the filter settings
  buffer_mult_of_input = min_samps / stream_n_samps_per_chan;
  if( (min_samps % stream_n_samps_per_chan) > 0)
    buffer_mult_of_input += 1;

  // set the total length of the filtered buffer (in samps per chan)
  buf_len = buffer_mult_of_input * stream_n_samps_per_chan;
  

  // get a copy of the right named filter from filt_map
  if(filt_map.find(tmp_filt_name) == filt_map.end()){
    std::cerr << "Couldn't find filter named: " <<
      tmp_filt_name << std::endl;
    exit(1);
  }
  my_filt = filt_map.find(tmp_filt_name)->second;

  // determine how many samples need to be in the filtered buffer
  // to accomodate those filter settings
  int min_samps_for_filt = my_filt.order;
  my_filt.buffer_mult_of_input = min_samps_for_filt / stream_n_samps_per_chan; // add 2 for safety?
  if( (min_samps_for_filt % stream_n_samps_per_chan) > 0 )
    my_filt.buffer_mult_of_input += 1;

  if(my_filt.buffer_mult_of_input < 2)
    my_filt.buffer_mult_of_input = 2;

  // filter may 'filtfilt' meaning run the same filter forward
  // and backwards on teh data.  This means we need to collect
  // more data before running the reverse filter (or else
  // we run the filter over a jump from zeros to real data -
  // which isn't ok).  So here we lengthen the circular
  // buffer accordingly.
  my_filt.buffer_mult_of_input += my_filt.filtfilt_wait_n_buffers;

  my_filt.buffer_mult_of_input +=2;  // lengthen the buffer, doesn't hurt
  my_filt.n_samps_per_chan = stream_n_samps_per_chan * my_filt.buffer_mult_of_input;
  
  // increase the filtered buffer length if filter needs it to be longer
  // this in unlikely for iir's which need only 4-8 samps, but possible
  // in the fir case
  if( my_filt.n_samps_per_chan > buf_len){
    buf_len = my_filt.n_samps_per_chan;
  }

  // set our raw pointerpointer to the daq's data pointer
  // the 2 levels of indirection is helpful in the case
  // of reading data from a file instead nof the daqs
  // (see neural_daq.cpp to understand this
  ptr_to_raw_stream = &(my_daq->data_ptr);

  // open files if we're writing to files
  finalize_done = true; // this flag will change if we open files
  if(! ((strcmp(buffer_dump_filename,"none"))==0) )
    init_files(tmp_buffer_filename);

  // set the cursurs to the beginning of the buffer
  // the start 1 input buffer behind 0.  This way, when
  // a new buffer is sent to filtered_buffer, *_curs
  // increments by the input buffer length to 0.
  // And after filtering, 0 is the first valid sample.
  u_curs =  buf_len - stream_n_samps_per_chan;
  f_curs =  buf_len - stream_n_samps_per_chan;
  ff_curs = buf_len - stream_n_samps_per_chan;

  // initialize i_inds.  These are samp numbers of the first chan,
  // first samp, first buffer of each of the 3 SOS superbuffers
  // (1st is u_buf, second is an intermediate, third is f_buf)
  // (due to order-4 iir filters going by 2 steps of
  // second-order-section filtering
  for(int i = 0; i < (MAX_FILTERED_BUFFER_N_INTERMEDIATE_BUFFERS + 2); i++){
    i_ind[i] = ( i * n_chans * buf_len );
  }
  //u_buf = &(i_buf[ i_ind[0] ]);
  //f_buf = &(i_buf[i_ind[ my_filt.filt_num_sos ] ]);

}

void Filtered_buffer::init_files(std::string &filename){

  finalize_done = false;
  std::string fn1 (filename);
  std::string fn2 (filename);
  fn1.append(".unfilt");
  fn2.append(".filt");
  buffer_dump_file_uf = try_fopen( fn1.c_str(), "wb" );
  buffer_dump_file_f  = try_fopen( fn2.c_str() , "wb" );

  file_buffer_count = 0;

  try_fwrite <uint32_t>( &file_buffer_count,       1, buffer_dump_file_uf );
  try_fwrite <uint16_t>( &n_chans,                 1, buffer_dump_file_uf );
  try_fwrite <uint16_t>( &stream_n_samps_per_chan, 1, buffer_dump_file_uf );
  
  try_fwrite <uint32_t>( &file_buffer_count,       1, buffer_dump_file_f );
  try_fwrite <uint16_t>( &n_chans,                 1, buffer_dump_file_f );
  try_fwrite <uint16_t>( &stream_n_samps_per_chan, 1, buffer_dump_file_f );

}

void Filtered_buffer::write_buffers(){
  
  if( buffer_dump_file_uf != NULL){
    file_buffer_count++;

    // For now, we are ignoring filtfilt buffer
    // Just write unfiltered buffer and filtered buffer
    try_fwrite <rdata_t>( &(i_buf[i_ind[0] + u_curs * n_chans]), 
			  stream_n_samps_per_chan * n_chans,
			  buffer_dump_file_uf );
    
    try_fwrite <rdata_t>( &(i_buf[i_ind[my_filt.filt_num_sos] + u_curs * n_chans]),
			  stream_n_samps_per_chan * n_chans,
			  buffer_dump_file_f );
  }
}

void Filtered_buffer::finalize_files(){

  if( buffer_dump_file_uf != NULL){
    std::cout << "Finalizing Files!  file_buffer_count = " << file_buffer_count << std::endl;
  
    rewind( buffer_dump_file_uf );
    try_fwrite <uint32_t> ( &file_buffer_count, 1, buffer_dump_file_uf );
    
    rewind( buffer_dump_file_f );
    try_fwrite <uint32_t> ( &file_buffer_count, 1, buffer_dump_file_f );
    
    finalize_done = true;
  }

}


