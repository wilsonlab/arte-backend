#include "lfp_bank.h"
#include <iostream>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <assert.h>
#include "filter_fun.h"
#include "netcom.h"
#include "datapacket.h"

extern FILE *main_file;
extern bool arte_disk_on;
extern pthread_mutex_t main_file_mutex;

Lfp_bank::Lfp_bank(){
  std::cout << "Lfp_bank constructor called." << std::endl;
  has_sockfd = false;
}

Lfp_bank::~Lfp_bank(){
  std::cout << "Lfp_bank destructor called." << std::endl;
  print_options();
  //if (has_sockfd)
  //  close(my_netcomdat.sockfd);
  //printf("caught an attempt to close sockfd\n");
}

int Lfp_bank::init(boost::property_tree::ptree &lfp_bank_pt,
		   std::map<int, neural_daq> &neural_daq_map,
		   std::map<std::string, Filt> &filt_map){

}

void Lfp_bank::init2(boost::property_tree::ptree &lfp_bank_pt,
		     Filtered_buffer * fb_curs){
  
  std::istringstream iss (lfp_bank_pt.data());
  iss >> lfp_bank_name;
  assign_property<uint16_t>("n_chans",         &n_chans,         lfp_bank_pt, lfp_bank_pt, 1);
  assign_property<uint16_t>("keep_nth_sample", &keep_nth_sample, lfp_bank_pt, lfp_bank_pt, 1);
 
  my_buffer = fb_curs;
  my_buffer->init(lfp_bank_pt, lfp_bank_pt, 0);

  // fix keep_nth_sample to evently divide the input buffer
  while( (my_buffer->stream_n_samps_per_chan % keep_nth_sample) > 0 ){
    printf("\a");
    std::cout << "(stream_n_samps_per_chan=" << my_buffer->stream_n_samps_per_chan <<
      ") % (keep_nth_sample=" << keep_nth_sample << ") = " <<
      my_buffer->stream_n_samps_per_chan % keep_nth_sample << ".  Decrimenting by 1 to " <<
      (keep_nth_sample - 1) << std::endl;
    keep_nth_sample--;
  }
  
  d_buf_len = my_buffer->stream_n_samps_per_chan / keep_nth_sample;

  total_buffers = 0;
  
  // initialize network
  my_netcom = new NetCom;
  std::string net_on;
  std::string host_ip;
  int port_num;
  char host_str[INET6_ADDRSTRLEN];
  assign_property<std::string>("network",   &net_on,   lfp_bank_pt, lfp_bank_pt, 1);
  assign_property<std::string>("host_ip",  &host_ip,  lfp_bank_pt, lfp_bank_pt, 1);
  assign_property<int>        ("port", &port_num, lfp_bank_pt, lfp_bank_pt, 1);
  strcpy( host_str, host_ip.c_str() );
  if( strcmp("on", net_on.c_str()) == 0){
    printf("lfp bank trying init with host %s and port %d\n", host_str, port_num);
    my_netcomdat = my_netcom->initUdpTx( host_str, port_num );
    printf("Successfully connected lfp_bank %d to host_ip %s at port %d . sockfd is %d\n",
	   lfp_bank_name, host_str, port_num, my_netcomdat.sockfd);
  }
  next_ts_ok_to_print = 0;
}

void Lfp_bank::print_options(void){
  std::cout << "still got to implement Lfp_bank::print_options()" << std::endl;
}

void *lfp_bank_filter_data(void *lfp_bank_in){    

  Lfp_bank * this_bank = (Lfp_bank*)lfp_bank_in;
  int i,c,samp_ind;

  filter_buffer( this_bank->my_buffer );

  // filtered_buffer::write_buffers() checks the flag for buffer dump to file
  // and writes to the dump file accordingly
  // shouldn't this be called from within filter_buffer?
  this_bank->my_buffer->write_buffers();
  
  // Now copy from the filtered buffer into the downsampled buffer
  for(i = 0; i < this_bank->d_buf_len; i++){
    for(c = 0; c < this_bank->n_chans; c++){
      samp_ind = i * this_bank->keep_nth_sample + this_bank->my_buffer->f_curs;
      this_bank->d_buf[ (i*(this_bank->n_chans)) + c ] = 
	this_bank->my_buffer->f_buf[ (samp_ind*(this_bank->n_chans)) + c ];
    }
  }
  
  lfp_bank_write_record(this_bank);

}

// I don't think that this needs to take void pointer anymore.
// can be lfp_bank pointer.
void lfp_bank_write_record(void *lfp_bank_in){

  //std::cout << "in write_record\n";
  Lfp_bank* this_bank = (Lfp_bank*) lfp_bank_in;
  uint16_t recordSizeBytes = 0;
  //std::cout << std::setw(6);

  if( this_bank->my_buffer->my_daq->buffer_timestamp < (this_bank->next_ts_ok_to_print - 5000))
    this_bank->next_ts_ok_to_print = this_bank->my_buffer->my_daq->buffer_timestamp;

  if(((this_bank->my_buffer->my_daq->buffer_timestamp > this_bank->next_ts_ok_to_print) ) &
     (this_bank->my_buffer->my_daq->buffer_timestamp < (UINT32_MAX-10000))){
    
    this_bank->next_ts_ok_to_print = this_bank->my_buffer->my_daq->buffer_timestamp + 5000;
    for(int s = 0; s < this_bank->d_buf_len; s++){
      std::cout << this_bank->my_buffer->my_daq->buffer_timestamp / 10000 << " ";
      for(int c = 0; c < this_bank->n_chans; c++){
	std::cout << this_bank->d_buf[(s * (this_bank->n_chans) ) + c] << " ";
      }
      std::cout << std::endl;
    }
    
  }
  char buff[4000];
  uint16_t temp = 2;
  lfp_bank_net_t lfp;
  int buff_size;

  // adjust the buffer timestamp so that it corresponds to the 
  // first sample in this buffer
  lfp.ts = this_bank->my_buffer->my_daq->buffer_timestamp;

  lfp.name = this_bank->lfp_bank_name;
  lfp.n_chans = this_bank->n_chans;
  lfp.n_samps_per_chan = this_bank->d_buf_len;
  lfp.samp_n_bytes = temp;
  int n_bytes = lfp.n_samps_per_chan * lfp.n_chans * lfp.samp_n_bytes;
  memcpy(lfp.data,  this_bank->d_buf, n_bytes);
  memcpy(lfp.gains, this_bank->d_buf, (lfp.n_samps_per_chan * lfp.n_chans)); // temproray fix b/c don't have a gains field in lfp_bank yet.
  
  // only save and transmit buffers with valid ts
  // (ts == 0 often
  if(lfp.ts > 0 & lfp.ts < (UINT32_MAX - 10000)){

    lfp.seq_num = this_bank->total_buffers;
    (this_bank->total_buffers)++;
    // send the wave to the network
    waveToBuff(&lfp, buff, &buff_size, true);
    NetCom::txBuff(this_bank->my_netcomdat, buff, buff_size);
    
    // save the wave to the disk
    waveToBuff(&lfp, buff, &buff_size, false);
    
    if(arte_disk_on){
      pthread_mutex_lock(&main_file_mutex);
      try_fwrite <char> (buff, buff_size, main_file);
      pthread_mutex_unlock(&main_file_mutex);
    }

    // printf("buff: ");
    //  for(int s = 0; s < 50; s++)
    //  printf("%c", buff[s]);
    //printf("\n");
  }
}

void Lfp_bank::end_acquisition(){
  if(has_sockfd)
    close(my_netcomdat.sockfd);
}
