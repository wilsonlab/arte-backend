// object represents 1 tetrode.  Takes raw data in full buffers, filters the needed channels,
// stores the filtered data, scans it for spikes, sends the spikes to the tubes, and to
// central file-saving object

#include "trode.h"
#include "util.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <iostream>
#include <stdint.h>

#include "filter_fun.h"

int tmp;
extern FILE *main_file;
extern pthread_mutex_t main_file_mutex;
extern bool arte_disk_on;

Trode::Trode(){
  has_sockfd = false;
  next_ok_spike_ts = 0;
  //std::cout << "In a trode constructor" << std::endl;
}

Trode::Trode( ArteSessionOptPb &session_pb ){
  Trode::Trode();
}

Trode::~Trode(){
  // if (has_sockfd)
  //  close(my_netcomdat.sockfd);
}

// New init function for the global array situation
void Trode::init2(boost::property_tree::ptree &trode_pt,
		  boost::property_tree::ptree &default_pt,
		  Filtered_buffer * fb_curs){

  std::string tmp_spikemode;
  // assign trode-specific properties
  assign_property<uint16_t>("n_chans", &n_chans, trode_pt, default_pt, 1);
  assign_property<uint16_t>("gains", gains, trode_pt, default_pt, n_chans);
  assign_property_ftor<rdata_t>("thresholds",  thresholds, trode_pt, default_pt, n_chans);
  assign_property<uint16_t>("samps_before_trig", &samps_before_trig, trode_pt, default_pt, 1);
  assign_property<uint16_t>("samps_after_trig" , &samps_after_trig , trode_pt, default_pt, 1);
  assign_property<uint16_t>("refractory_period_samps",&refractory_period_samps,trode_pt,default_pt,1);
  assign_property<std::string>("spike_mode"    , &tmp_spikemode    , trode_pt, default_pt, 1);
  strcpy( spike_mode, tmp_spikemode.c_str() );

  n_samps_per_spike = samps_before_trig + samps_after_trig + 1;
  my_buffer = fb_curs;
  total_spikes = 0;

  // Filtered_buffer::init() should take 2 property trees and a min sample number
  // We need at very least n_samps_before + n_samps_after + 1 for the trig sample + 1 for padding
  // NB check the reasoning for the above count.  Do we need a longer buffer?
  my_buffer->init(trode_pt, default_pt, (n_samps_per_spike + 1) );

  // Init the spike-detector.  Pass my_buffer in
  // Why not pass in the trode?  because then spike_detector.h would have to include trode.h
  // Cant do that because trode.h includes spike_detector.h  Spike-detector will init
  // as much as it can from the buffer settings.  Trode-specific stuff will have to be
  // passed in through additional arguments to spike_detector::init()
  //my_spike_detector.init(my_buffer);

  refractory_period_tics = refractory_period_samps / (uint16_t)SAMPLES_PER_TIC;

  setup_spike_array();
    
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
    
    printf("trode name %d trying init with host %s and port %d\n", name, host_str, port_num);
    my_netcomdat = my_netcom->initUdpTx( host_str, port_num );
    printf("Successfully connected tetrode %d to host_ip %s at port %d. sockfd is %d\n",
	   name, host_ip.c_str(), port_num, my_netcomdat.sockfd);
    //has_sockfd = true;
  }

  // make a list of ints to count up how many times each samp in the buffer has been searched for spikes
  //n_times_checked = new int [ my_buffer->buf_len ];
  //for(int z = 0;  z <  my_buffer->buf_len; z++){
  //  n_times_checked[z] = 0;
  //}
  next_ok_spike_ts = 0;

}

void Trode::setup_spike_array(){

  // fill out the spike_net packets - the parts that we know
  for(int n = 0; n < MAX_N_SPIKES_PER_BUFFER; n++){
    
    spike_array[n].ts = -1;
    spike_array[n].name = name;
    spike_array[n].n_chans = n_chans;
    spike_array[n].n_samps_per_chan = n_samps_per_spike;
    spike_array[n].samp_n_bytes = 2;
    for(int m = 0; m < (n_chans * n_samps_per_spike); m++)
      spike_array[n].data[m] = m;
    for(int m = 0; m < n_chans; m++)
      spike_array[n].gains[m] = 1;
    for(int m = 0; m < n_chans; m++)
      spike_array[n].thresh[m] = thresholds[m];
    spike_array[n].trig_ind = samps_before_trig;
    spike_array[n].seq_num = 0;
  }
  printf("Done initializing spike structs.\n");
}

void Trode::set_thresh_uv_one_thresh_n_chans(int16_t uv_thresh, int *chan_array, int n_chans_to_set){

  double thresh_at_daq_V;
  int target_chans[MAX_FILTERED_BUFFER_N_CHANS];
  int n_target;
  int i;
 
  if(n_chans_to_set == -1){
    n_target = n_chans;
    for(i = 0; i < n_target; i++){
      target_chans[i] = i;
    }
  } else {
    n_target = n_chans_to_set;
    for(i = 0; i < n_target; i++){
      if( chan_array[i] < n_chans ){
	target_chans[i] = chan_array[i];
      } else {
	printf("Error: tried to set chan %d of trode %d which has %d chans\n", 
	       chan_array[i], name, n_chans);
	target_chans[i] = i;
      }
    }
  }

  for(i = 0; i < n_target; i++){
    thresh_at_daq_V = (double)uv_thresh / 1000000.0 * (double)gains[i];
    thresholds[i] = ftor(&thresh_at_daq_V);
  }
}

void Trode::set_thresh_uv_n_thresh_n_chans(int16_t *uv_thresh_array, int *chan_array, int n_chans_to_set){
  for(int i = 0; i < n_chans_to_set; i++){
    set_thresh_uv_one_thresh_n_chans( uv_thresh_array[i], chan_array+i , 1 );
    //set_thresh_uv_one_thresh_n_chans( 1, chan_array, 1 );
  }
}

void Trode::set_thresh_uv_one_thresh_all_chans(int16_t uv_thresh){
  // set all chans to this thresh
  set_thresh_uv_one_thresh_n_chans( uv_thresh, NULL, -1 );
}
    

void *trode_filter_data(void *t){

  int n_spikes;

  filter_buffer( ((Trode *)t)->my_buffer );

  ((Trode*)t)->my_buffer->write_buffers();

  // We only look for spikes once the buffer ts has come to positive values
  if( ((Trode*)t)->my_buffer->my_daq->buffer_timestamp > 0){
    n_spikes = find_spikes( (Trode*)t );
  }

  if(n_spikes > 0){
    for(int n = 0; n < n_spikes; n++){
      //printf("FOUND SPIKE!\n");
      if( (((Trode*)t)->spike_array[n].ts >= ((Trode*)t)->next_ok_spike_ts) |
	  (  ((Trode*)t)->spike_array[n].ts < ((Trode*)t)->next_ok_spike_ts - 5000 )){

	// The first case admits spikes that happened after next_ok_spike_ts (last spike time + refractory)
	// The second case admits spikes that are more than half a millisecond OLDER than next_ok
	// This allows through spikes that came after a clock_restart

	//if( ((Trode*)t)->name == 0 & true){
	  //printf("next_ok_ts: %d  this_ts called ok: %d\n", ((Trode*)t)->next_ok_spike_ts, ((Trode*)t)->spike_array[n].ts);
	  //printf("*************SENDING A SPIKE***************\n");
	//}
	
	if(false & ((Trode*)t)->name == 0)
	  printf("Got inside the if block.\n");

	((Trode*)t)->spike_array[n].seq_num = ((Trode*)t)->total_spikes;
	((Trode*)t)->total_spikes++;

	spike_to_disk(&((Trode*)t)->spike_array[n]);
	spike_to_net(&((Trode*)t)->spike_array[n], (Trode*)t);
	((Trode*)t)->next_ok_spike_ts = ((Trode*)t)->spike_array[n].ts + ((Trode*)t)->refractory_period_tics;
	
	//if( ((Trode*)t)->name == 0 & true){
	// printf("sent the spike at %d.  Now next_ok_ts is %d.\n",n, ((Trode*)t)->next_ok_spike_ts);
	//}

      }
    }
  }

//if( ((Trode*)t)->name == 0 & n_spikes > 0)
//   printf("Found %d spikes.\n",n_spikes);

}


int find_spikes(Trode *t){

  int n_spikes = 0;
  int spike_starts[MAX_N_SPIKES_PER_BUFFER];
  int buf_len, n_chans, search_start_ind, start_c, search_stop_ind, stop_c, stop_ind; 
  int samps_before_trig, samps_after_trig, n_samps_per_spike, search_cursor, search_c, prev_c,  s, c;
  bool found_spike;
  rdata_t *thresh, *spike_buffer, *f_buf;
  Filtered_buffer *fb;
  //SpikeDetector *sd;
  int n_samps_checked;

  //  printf("in find_spikes\n");
  //  fflush(stdout);

  fb = t->my_buffer;
  buf_len = fb->buf_len;
  n_chans = t->n_chans;
  n_samps_per_spike = t->n_samps_per_spike;
  samps_before_trig = t->samps_before_trig;
  samps_after_trig = t->samps_after_trig;
  f_buf = &(fb->f_buf[0]);
  thresh = (t->thresholds);
  
  search_stop_ind  =  (fb->f_curs + fb->stream_n_samps_per_chan - t->samps_after_trig);
  //search_start_ind = (search_stop_ind - t->n_samps_per_spike);
  search_start_ind = (search_stop_ind - fb->my_daq->n_samps_per_buffer + 1);
  
  n_samps_checked = 0;
  
  for(search_cursor = search_start_ind; search_cursor <= search_stop_ind; search_cursor++){
    
    search_c = CBUF(search_cursor,   buf_len);
    prev_c   = CBUF(search_cursor-1, buf_len);
    
    //t->n_times_checked[search_c]++;

    if(false){
      printf("stop_ind: %d   start_ind:%d   search_cursor:%d\n samp1:%d  2:%d  3:%d  4:%d  thresh1:%d  2:%d   3:%d  4:%d\n",
	     search_stop_ind, search_start_ind, search_cursor,
	     f_buf[search_c * n_chans + 0], f_buf[search_c * n_chans + 1],
	     f_buf[search_c * n_chans + 2], f_buf[search_c * n_chans + 3],
	     thresh[0], thresh[1], thresh[2], thresh[3]);
      fflush(stdout);
    }
    
    found_spike = 0;
    for(c = 0; c < n_chans; c++){
      if( (f_buf[ search_c*n_chans + c ] >= thresh[c]) & (f_buf[prev_c*n_chans + c] < thresh[c]) )
	found_spike++;
    }
    if( t->name == 0 & false & found_spike){
      printf("Found a spike: search_curs:%d search_c:%d samp-thresh:%d   last_search:%d last_c:%d lastsamp-thresh:%d\n",
	     search_cursor, search_c, f_buf[search_c*n_chans + 0] - thresh[0],
	     search_cursor-1, prev_c, f_buf[prev_c*n_chans + 0] - thresh[0]);
    }
    
    if(found_spike > 0){
      
      if(false){
	printf("FOUND SPIKE! refrac peridod samps:%d  start_ind:%d   stop_ind:%d n_samps_per_spike:%d\n",
	       t->refractory_period_samps, search_start_ind, search_stop_ind, t->n_samps_per_spike);
	fflush(stdout);
      }
      
      //t->spike_array[n_spikes].ts = t->my_buffer->my_daq->buffer_timestamp + (search_c * 10)/32;
      
      //t->spike_array[n_spikes].ts = t->my_buffer->my_daq->buffer_timestamp +
      //CBUF( search_cursor - fb->f_curs, fb->buf_len)*10/32;
      
      t->spike_array[n_spikes].ts = t->my_buffer->my_daq->buffer_timestamp +
      	(search_cursor - fb->f_curs)*10/32;
      
      //t->spike_array[n_spikes].ts = t->my_buffer->my_daq->buffer_timestamp + 
      //(search_c - fb->f_curs)*10/32;
      
      if(t->name == 0 & false){
        printf("Figuring ts. f_curs:%d buf_len:%d search_cursor:%d search_c:%d buffer_ts:%d  (search_cursor - f_curs):%d   (search_cursor-f_curs)10/32:%d\n",
	       fb->f_curs, fb->buf_len, search_cursor, search_c, t->my_buffer->my_daq->buffer_timestamp, (search_cursor - fb->f_curs), (int)((search_cursor - fb->f_curs)*10/32));
      }

      if(t->name == 0 & false){
	printf("buffer ts is %d.  Spike ts is %d\n", t->my_buffer->my_daq->buffer_timestamp,
	       t->spike_array[n_spikes].ts);
	printf("n_times_checked: ");
	for(int i = 0; i < fb->buf_len; i++){
	  printf("%d ", t->n_times_checked[search_c]);
	}
	printf("\n");
      }

      for(s = 0; s < n_samps_per_spike; s++){
 	for(c = 0; c < n_chans; c++){
 	  t->spike_array[n_spikes].data[ s * n_chans + c ] = f_buf[ CBUF((search_cursor-samps_before_trig+s), buf_len)*n_chans + c];
 	}
      }
      
      n_spikes++;
      
      if(false & t->name == 0){
	printf("f_curs %d, start_ind %d, stop_ind %d, search_cursor before update: %d\n",
	       fb->f_curs, search_start_ind, search_stop_ind, search_cursor );
	fflush(stdout);
      }
      //pritnf("search cursor before update: %d\n",search_cursor);
      search_cursor = search_cursor + t->refractory_period_samps - 1; // drop 1 1b/c we add the 1 in the for loop
      if(false & t->name == 0){
	printf("f_curs %d, start_ind %d, stop_ind %d, search_cursor after update: %d\n\n",
	       fb->f_curs, search_start_ind, search_stop_ind, search_cursor);
	fflush(stdout);
      }
      if(false & t->name == 0 & n_spikes > 0){
	printf("spike ts:%d   next_ok_spike_ts:%d buffer_ts:%d\n",
	       t->spike_array[n_spikes-1].ts, t->next_ok_spike_ts, t->my_buffer->my_daq->buffer_timestamp);
      }
    }
    
  }
  return n_spikes;  // so the caller knows how much of the spike_array has been filled
}


void spike_to_disk(spike_net_t *spike){
  char buff[4000]; // TODO: get the right buffer size
  int buff_size, n;
  // TODO: Get this part thread-safe, needz mutex
  
  spikeToBuff(spike, buff, &buff_size, false);
  
  if(arte_disk_on){
    pthread_mutex_lock(&main_file_mutex);
    try_fwrite <char> (buff, buff_size, main_file);
    pthread_mutex_unlock(&main_file_mutex);
  }
}

void spike_to_net(spike_net_t *spike, Trode *t){
  
  
  if(true){
    char buff[4000];
    int buff_size;
    spikeToBuff(spike, buff, &buff_size, true);
    
    if(false){
      printf("buffsize: %d\n",buff_size);
      for(int n = 0; n < buff_size; n++)
	printf("%c",buff[n]);
      printf("\n");
    }
    //printf("name %d  ip %s    port %d  sockfd %d \n",t->name, t->my_netcomdat.host_ip, t->my_netcomdat.port, t->my_netcomdat.sockfd);
    
    if(false){
      printf("name %d  sockfd %d\n", t->name, t->my_netcomdat.sockfd);
      fflush(stdout);
    }
    
    NetCom::txBuff(t->my_netcomdat, buff, buff_size);
  }    
  
  if(false){
    NetCom::txSpike(t->my_netcomdat, spike);
  }
  
}

void Trode::end_acquisition(){
  if(has_sockfd)
    close(my_netcomdat.sockfd);
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

}


void Trode::print_spikes(void){

}
