#include "trode.h"
#include "arteopt.h"
#include "global_defs.h"
#include "filt.h"
#include <NIDAQmx.h>
#include <map>
#include <iostream>

int main(int argc, char *argv[]){

  arte_init(argc, argv, default_setup_config_filename, default_session_config_filename);

  int n_chans = trode_map["tt00"].trode_opt.n_chans;
  int buffer_len = trode_map["tt00"].my_daq->n_samps_per_buffer;
  int n_buffers_to_make = 1;

  float64 A = 2.0;
  int T = 32; // square wave period in sample units
  
  float64 *this_buffer;
  std::cout << "old ptr: " << trode_map["tt00"].my_daq->data_ptr << " ";
  
  this_buffer = new float64 [MAX_TRODE_BUFFER]; // extra-big buffer, b/c of the way Trode::print_buffers steps way over the edge of the daq buffer
  init_array <float64> (this_buffer, 6.0, n_chans * buffer_len);
  
  trode_map["tt00"].my_daq->data_ptr = this_buffer;
  trode_map["tt00"].my_daq->n_chans = n_chans;
  //trode_map["tt00"].my_daq->data_ptr_copy = this_buffer;
  //trode_map["tt00"].my_daq->copy_flexptr = this_buffer;
  //trode_map["tt00"].pp_to_raw_stream = &this_buffer;
  
  std::cout << "local ptr: " << this_buffer << "  new daq ptr: " << trode_map["tt00"].my_daq->data_ptr << std::endl;

  for(int s = 0; s < buffer_len; s++){
    int h = s * n_chans;
    for(int c = 0; c < n_chans; c++){
      if( (s % T) < (T/2) ) {
	this_buffer[h + c] = -1*A;
      }
      else{
	this_buffer[h + c] = A;
      }
    }
  }
  //  memcpy( trode_map["tt00"].my_daq->data_ptr_copy, this_buffer, (n_chans * buffer_len * 8) );
  trode_filter_data(&(trode_map["tt00"]));
  trode_map["tt00"].print_buffers(4,102);

  return 0;
}
