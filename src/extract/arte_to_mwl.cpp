#include "arte_to_mwl.h"
#define _FILE_OFFSET_BITS 64

#define MAX_PACKET_BYTES 8000

bool permissive_buffer_lengths = false;
bool auto_drop_bad_length_packets = true;
int bad_length_packets;

FILE *in_f, *out_f;
int spike_count;

bool interactive;
bool verbose;

// Set a first-buffer time threshold in order to drop spikes occurring too early
// (buffers suffering from the uint underwrite)
uint32_t low_time_threshold = 1000000; // 1,000,000 time units = 100 seconds
bool encountered_low = false;

spike_net_t g_spike;    // global spike struct
lfp_bank_net_t g_lfp;   // global lfp struct

uint16_t show_header(bool pr) {
  char buff_head[4];
  char the_type;
  uint16_t the_length;
  char the_next_byte;

  fread  (buff_head, sizeof(char), 4,  in_f);
  fseek( in_f, -4, SEEK_CUR );
  
  the_type = buff_head[0];
  memcpy( &the_length, buff_head+1, 2 );
  memcpy( &the_next_byte, buff_head+3, 1);
  
  if(pr)
    printf("Current buffer header:\n the_type: %c\n the_length: %d\n the_next_byte: %d\n\n", the_type, the_length, the_next_byte);

  return the_length;
}

int main(int argc, char *argv[]){

  void *this_arte_packet;
  this_arte_packet = malloc(MAX_PACKET_BYTES);

  int sourcename;
  packetType_t sourcetype;
  int ok_packet;
  int packet_count = 0;
  char blank_data[MAX_PACKET_BYTES];
  
  bad_length_packets = 0;
  //this_arte_packet = (void *)&(blank_data[0]);

  char input_filename[200], output_filename[200];

  init_filenames(argc, argv, 
		 input_filename, output_filename, 
		 &sourcename, &sourcetype);

  printf("sourcename:%d sourcetype:%c spike_sourcetype:%c\n", 
	 sourcename, sourcetype, NETCOM_UDP_SPIKE);


    if(this_arte_packet == NULL){
    printf("Memory error, sorry :[\n");
    exit(1);
  }

  in_f = fopen(input_filename, "rb");
  out_f = fopen(output_filename, "wb");

  if(in_f == NULL)
    printf("Bad in file: %s\n", input_filename);
  if(out_f == NULL)
    printf("Bad out_file: %s\n", output_filename);
  
  if(in_f == NULL || out_f == NULL)
    exit(1);

  ok_packet = false;
  while(!ok_packet & (feof(in_f) == 0) ){
    ok_packet = get_next_packet(this_arte_packet, sourcename, sourcetype);
  }
  printf("finished looking...\n");  fflush(stdout);

  write_file_header(this_arte_packet, argc, argv, sourcetype);
  
  printf("Finished writing file header.\n"); fflush(stdout);
  
  if( sourcetype == NETCOM_UDP_SPIKE){
    spike_net_t* test_spike = (spike_net_t*)this_arte_packet;
    printf("test before first write_mwl: name:%d n_chans:%d\n", 
	   test_spike->name, test_spike->n_chans);
  }
  
  write_mwl(this_arte_packet, sourcetype);
  
  printf("Finished writing first packet with write_mwl.\n"); fflush(stdout);

  if( sourcetype == NETCOM_UDP_SPIKE){
    spike_net_t* test_spike = (spike_net_t*)this_arte_packet;
    printf("test before loop: name:%d n_chans:%d\n", 
	   test_spike->name, test_spike->n_chans);
  }
  
  if(true){
    
    while( feof(in_f) == 0){
      ok_packet = get_next_packet(this_arte_packet, sourcename, sourcetype);
      if(verbose){
	if (ok_packet){
	  printf("This packet was ok:\n");}
	else {
	  printf("This packet was not ok:\n");}
	
	print_packet(this_arte_packet, sourcetype);
      }
      
      if(ok_packet){
	
	//print_timestamp(this_arte_packet, sourcetype);

	if(bad_length_packets > 0){
	  printf("Writing a packet at some point later than the corruption.  Writing:\n");
	  print_packet(this_arte_packet, sourcetype);
	}

	fflush(out_f);
	//printf("in loop...\n");
	if( sourcetype == NETCOM_UDP_SPIKE){
	  spike_net_t* test_spike = (spike_net_t*)this_arte_packet;
	  //printf("test before write_mwl in loop: name:%d n_chans:%d\n", 
	  //	 test_spike->name, test_spike->n_chans);
	}
	
   	write_mwl(this_arte_packet, sourcetype);
	packet_count++;
      }
      interactive_wait("message1\n");
    }
    
  } // end if(true)
    //free(this_arte_packet);

  printf("finished. Wrote %d packet(s)\n", packet_count);
  printf("Encountered %d bad_length_packets.\n", bad_length_packets);
  exit(0);
  
}

void write_mwl(void *arte_packet, packetType_t sourcetype){
  
  spike_net_t *write_spike;
  lfp_bank_net_t *write_lfp;
  
  if( sourcetype == NETCOM_UDP_SPIKE ){
    write_spike = (spike_net_t *)arte_packet;
    //printf("In write_mwl: spike name:%d n_chans:%d\n", write_spike->name, write_spike->n_chans);
    fwrite(&(write_spike->ts), 1, sizeof(write_spike->ts), out_f);
    fwrite(write_spike->data, (write_spike->n_chans * write_spike->n_samps_per_chan), sizeof(rdata_t),  out_f);
    return;
  }

  if( sourcetype == NETCOM_UDP_LFP ){
    write_lfp = (lfp_bank_net_t *)arte_packet;
    fwrite( &(write_lfp->ts), 1, sizeof(timestamp_t), out_f );
    fwrite( write_lfp->data, write_lfp->n_chans * write_lfp->n_samps_per_chan,  sizeof(rdata_t), out_f);
    return;
  }

  printf("Tried to write a buffer of unknown type\n");
  exit(1);

}

void ps(){
  fprintf(out_f,"%%"); // %% seems to be the escape sequence for getting a % printed
}

void write_file_header(void *arte_packet, int argc, char *argv[], packetType_t sourcetype){

  printf("addy of arte packet from pov of write_header:%p\n",arte_packet);fflush(stdout);

  spike_net_t *spike;
  lfp_bank_net_t *lfp;
  
  if( sourcetype == NETCOM_UDP_SPIKE){
    printf("DID THE ASSIGNMENT TO SPIKE\n");fflush(stdout);
    spike = (spike_net_t *)arte_packet;
    printf("IN WRITE HEADER: name:%d ts:%d n_chans:%d\n",spike->name, spike->ts, spike->n_chans); fflush(stdout);
  }
  if( sourcetype == NETCOM_UDP_LFP){
    lfp = (lfp_bank_net_t *)arte_packet;
  }

  //printf("TEST in write_file_header\n"); fflush(stdout);


  printf("EARLY-PRE-INTERMEDIATE TEST: spike->name:%d ts:%d n_chans:%d n_samps_per_chan:%d\n",
	 spike->name, spike->ts, spike->n_chans, spike->n_samps_per_chan); fflush(stdout);


  ps(); ps(); fprintf(out_f, "BEGINHEADER\n");
  ps(); fprintf(out_f, " Program:\tadextract\n"); //Problem?
  ps();   fprintf(out_f, " Program Version:\t1.18\n"); //Problem?
  ps(); fprintf(out_f, " Argc:\t%d\n", argc);
  for(int i = 1; i < argc; i++){
    ps(); fprintf(out_f, " Argv[%d] :\t%s\n", i, argv[i]); //Problem?
  }
  ps(); fprintf(out_f, " Date:\tTue Jun 21 09:53:08 2011\n"); //Problem?
  ps();  fprintf(out_f, " Directory:\t/home/greghale/data/joe/062011\n"); //Problem?
  ps(); fprintf(out_f, " Hostname:\tjellyroll\n"); //sloppy
  ps(); fprintf(out_f, " Architecture:\ti686\n"); 
  ps(); fprintf(out_f, " User:\tgreghale ()\n"); //sloppy
  ps(); fprintf(out_f, " File type:\tBinary\n");

  //printf("ANOTHER TEST\n"); fflush(stdout);

  if( sourcetype == NETCOM_UDP_SPIKE )  {
    ps(); fprintf(out_f, " Extraction type:\ttetrode waveforms\n"); }

  if( sourcetype == NETCOM_UDP_LFP ){
    ps(); fprintf(out_f, " Extraction type:\tcontinuous data\n"); }

  if( sourcetype == NETCOM_UDP_SPIKE) {
    ps(); fprintf(out_f, " Probe:\t0\n");}  //Problem?

  if( sourcetype == NETCOM_UDP_LFP) {
    ps(); fprintf(out_f, " Probe:\t-1\n"); }


  //  printf("PRE-INTERMEDIATE TEST: spike->name:%d ts:%d n_chans:%d n_samps_per_chan:%d\n",
  //	 spike->name, spike->ts, spike->n_chans, spike->n_samps_per_chan); fflush(stdout);

  
  if( sourcetype == NETCOM_UDP_SPIKE){
    ps(); fprintf(out_f, " Fields:\ttimestamp,%d,%d,%d\twaveform,%d,%d,%d\n",8,4,1,2,2,
		  spike->n_chans * spike->n_samps_per_chan);
    //printf("spike->ts:%d  spike->name:%d, spike->n_chans:%d, spike->n_samps_per_chan:%d\n",
    //	   spike->ts, spike->name, spike->n_chans, spike->n_samps_per_chan);
  }


  if( sourcetype == NETCOM_UDP_LFP){
    ps(); fprintf(out_f, " Fields:\ttimestamp,8,4,1\tdata,2,2,%d\n", 
		  lfp->n_chans * lfp->n_samps_per_chan);}

  ps(); fprintf(out_f, "\n");
  ps(); fprintf(out_f, " Beginning of header from input file 'raw/02200320.sgh'\n"); //Problem?

  if( sourcetype == NETCOM_UDP_SPIKE ){
    ps(); fprintf(out_f, " mode: SPIKE\n");}

  if( sourcetype == NETCOM_UDP_LFP ){
    ps(); fprintf(out_f, " mode: CONTINUOUS\n");}

  ps(); fprintf(out_f, " adversion:\t1.36b\n");

  if( sourcetype == NETCOM_UDP_SPIKE){
    ps(); fprintf(out_f, " rate:\t250000.000000\n");} //Problem?

  if( sourcetype == NETCOM_UDP_LFP){
    ps(); fprintf(out_f, " rate:\%d.00000\n", 8* lfp->n_samps_per_chan * 1000);}

  ps(); fprintf(out_f, " nelectrodes: 2\n");

  if( sourcetype == NETCOM_UDP_SPIKE){
    ps(); fprintf(out_f, " nchannels:\t8\n");} // Problem?

  if( sourcetype == NETCOM_UDP_LFP){
    ps(); fprintf(out_f, " nchannels:\t%d\n", lfp->n_chans);}

  if( sourcetype == NETCOM_UDP_SPIKE){
    ps(); fprintf(out_f, " nelect_chan:\t%d\n", spike->n_chans);} //Problem?

  if( sourcetype == NETCOM_UDP_LFP){
    ps(); fprintf(out_f, " nelect_chan:\t4\n");}

  //printf("INTERMEDIATE TEST: spike->name:%d ts:%d n_chans:%d n_samps_per_chan:%d\n",
  //	 spike->name, spike->ts, spike->n_chans, spike->n_samps_per_chan); fflush(stdout);

  ps(); fprintf(out_f, " errors:\t%d\n",0); // me?  errors?
  ps(); fprintf(out_f, " disk_errors:\t%d\n",0);
  ps(); fprintf(out_f, " dma_bufsize:\t%d\n",24567); //Problem?

  if( sourcetype == NETCOM_UDP_SPIKE ){
    ps(); fprintf(out_f, " spikelen:\t%d\n", spike->n_samps_per_chan);} //Problem?
  
  if( sourcetype == NETCOM_UDP_LFP ){
    ps(); fprintf(out_f, " spikelen:\t4\n");}

  if( sourcetype == NETCOM_UDP_SPIKE ){
    ps(); fprintf(out_f, " spikesep:\t%d\n", 32);} // sloppy.  Do I need to put refrac period in packets?

  if( sourcetype == NETCOM_UDP_LFP){
    ps(); fprintf(out_f, " spikesep:\t26\n");}



  if( sourcetype == NETCOM_UDP_SPIKE){
    int chan_offset[8] = {65, 122, 180, 238, 296, 353, 411, 469};
    for(int i = 0; i < 8; i++){
      //printf("test beginning i is %d\n",i); fflush(stdout);
      ps(); fprintf(out_f, " channel %d ampgain:\t%d\n", i, 10000); // sloppy
      ps(); fprintf(out_f, " channel %d adgain:\t%d\n", i, 0);
      ps(); fprintf(out_f, " channel %d filter:\t%d\n", i, 136); // somewhat sloppy (does spikeparms care about filter settings?)
      uint16_t this_thresh;
      //printf("spike->n_chans: %d\n", spike->n_chans); fflush(stdout);
      if(i <= spike->n_chans){
	this_thresh = spike->thresh[i];
      }else{
	this_thresh = 154;
      }
      //printf("middest i is %d\n",i); fflush(stdout);
      ps(); fprintf(out_f, " channel %d threshold:\t%d\n", i, this_thresh);
      ps(); fprintf(out_f, " channel %d color:\t%d\n",i, 15);
      ps(); fprintf(out_f, " channel %d offset:\t%d\n", i, chan_offset[i]);
      ps(); fprintf(out_f, " channel %d contscale:\t%d\n", i, 0);
      //printf("test end i is %d\n",i); fflush(stdout);
    }
    ps(); fprintf(out_f, " spike_size\t%d\n", 
		  (spike->n_chans * spike->n_samps_per_chan * 2 + 8)); // seemed to be the meaning from a real tt file?  is it important?
    ps(); fprintf(out_f, " fields: int electrode_num; long timestamp; int data[%d]\n",
		  spike->n_chans * spike->n_samps_per_chan);
  }

  //printf("ANOTHER TEST\n"); fflush(stdout);


  if( sourcetype == NETCOM_UDP_LFP ){
    for(int i = 0; i < lfp->n_chans; i++){
      ps(); fprintf(out_f, " channel %d ampgain:\t%d\n",i, lfp->gains[i]);
      ps(); fprintf(out_f, " channel %d adgain:\t0\n", i);
      ps(); fprintf(out_f, " channel %d filter:\t136\n", i); 
      // pat yourself on the back
      ps(); fprintf(out_f, " channel %d threshold:\t200\n", i);
      ps(); fprintf(out_f, " channel %d color:\t15\n", i);
      ps(); fprintf(out_f, " channel %d offset:\t65\n",i);
      ps(); fprintf(out_f, " channel %d contscal:\t0\n",i);
    }
    ps(); fprintf(out_f, " buffer_size\t%d\n", lfp->n_chans * lfp->n_samps_per_chan + 4);

    ps(); fprintf(out_f, " fields: long timestamp; int data[%d]\n", 
		  lfp->n_chans, lfp->n_samps_per_chan);
    
  }

  ps(); fprintf(out_f, " End of header from input file 'raw/02200320.sgh'\n"); //Problem?
  ps(); fprintf(out_f, "\n");
  ps(); ps(); fprintf(out_f, "ENDHEADER\n");
  
}

void interactive_wait(const char *wait_str){
  uint16_t the_length = 0;
  if(interactive){ 
    fflush(stdout);
    char c = 'b';
    printf("In interactive mode. hit a [enter] to advance 1 byte until reaching the_length 68 or 294, f [enter] to exit interactive mode.\n");
    printf("%s\n",wait_str);
    while(c != '\n'){
      c = getchar();
      if(c == 'f')
	interactive = false;
      if(c == 'a') {
	int search_dist = 0;
	the_length = show_header(true);
	printf("The_length: %d\n", the_length);
	while (the_length != 68 && the_length != 294){
	  search_dist++;
	  printf("Search dist: %d", search_dist);
	  fseek( in_f, 1, SEEK_CUR );
	  the_length = show_header(true);
	}
      }
    }
  }
  fflush(stdout);
}

void init_filenames(int argc, char *argv[], 
		    char input_filename[], char output_filename[], 
		    int *sourcename, packetType_t *sourcetype_p){
  

  interactive = false;
  verbose = false;

  if(argc < 7){
    printf("Argc was %d\n",argc);
    for(int i = 0; i < argc; i++){
      printf("Argv[%d] was: %s\n", i, argv[i]);
    }
    printf("Normal usage: \n");
    printf("arte_to_mwl -trodename 06 -i input_file.data -o output_file.tt [-interactive] [-verbose]\n");
    printf("or\n");
    printf("arte_to_mwl -lfpbankname 4 -i input_file.data -o output_file.eeg [-interactive] [-verbose]\n");
    exit(1);
  }

  for(int i = 1; i < argc; i++){
    if( strcmp( argv[i], "-i" ) == 0){
      strcpy( input_filename, argv[i+1]);
      i++;
      continue;
    }
    if( strcmp( argv[i], "-o") == 0){
      strcpy( output_filename, argv[i+1]);
      i++;
      continue;
    }
    if( strcmp( argv[i], "-trodename") == 0){
      sscanf(argv[i+1], "%d", sourcename);
      // Plz explain why netcom udp spike? (for documentation, readability)
      *sourcetype_p = NETCOM_UDP_SPIKE; 
      printf("Set the sourcetype to:%c\n", *sourcetype_p);
      i++;
      continue;
    }
    if (strcmp( argv[i], "-lfpbankname") == 0){
      sscanf(argv[i+1], "%d", sourcename);
      *sourcetype_p = NETCOM_UDP_LFP;
      i++;
      continue;
    }
    if (strcmp( argv[i], "-interactive") == 0){
      interactive = true;
      continue;
    }
    if (strcmp( argv[i], "-verbose") == 0){
      verbose = true;
      continue;
    }
    printf("Unrecognized argument:%s\n",argv[i]);
    exit(1);
  }
 
}

void print_timestamp(void *arte_packet, packetType_t sourcetype){
  if(sourcetype == NETCOM_UDP_SPIKE){
    spike_net_t *spike = (spike_net_t*) arte_packet;
    printf("Spike %d ts: %d\n", spike->name, spike->ts);
  }
  if(sourcetype == NETCOM_UDP_LFP){
    lfp_bank_net_t *lfp = (lfp_bank_net_t*) arte_packet;
    printf("Lfp %d ts: %d\n", lfp->name, lfp->ts);
  }
  if(sourcetype != NETCOM_UDP_SPIKE && sourcetype != NETCOM_UDP_LFP) {
    printf("UNKNOWN TYPE!\n");
  }
}

void print_packet(void *arte_packet, packetType_t sourcetype){
  
  if(true){

    // char packet_type;
    // uint32_t packet_size;
    // char header_blank;
    // memcpy( arte_packet,      &packet_type, 1 );
    // memcpy( arte_packet + 1,  &packet_size, 2 );
    // memcpy( arte_packet + 3,  &header_blank,1 );
    // printf("Printing a packet type:%c  size:%d  (int)blank:%d\n",
    // 	   packet_type, packet_size, header_blank);

    if (sourcetype == NETCOM_UDP_SPIKE){
      spike_net_t *spike = (spike_net_t*) arte_packet;
      printf("**Spike packet**\n");
      printf("Trode name:%d \nts: %d \nn_chans:%d \nn_samps_per_chan:%d \n",
	     spike->name, spike->ts, spike->n_chans, spike->n_samps_per_chan);
    } // end if spike
    
    else if (sourcetype == NETCOM_UDP_LFP){
      lfp_bank_net_t *lfp = (lfp_bank_net_t*) arte_packet;
      printf("**Lfp bank packet**\n");
      printf("Lfp_bank name:%d \nts: %d \nn_chans: %d \nn_samps_per_chan:%d \n",
	     lfp->name, lfp->ts, lfp->n_chans, lfp->n_samps_per_chan);
    }  // end if lfp_bank
    
    else{
      printf("Packet was neither spike nor lfp.\n");
    }  // end if other

  } // end if verbose
}


bool get_next_packet(void *arte_packet, int sourcename, packetType_t sourcetype){

  bool ok_packet;
  char buff_head[4];
  char buff[4000];
  char the_type;
  uint16_t the_length;
  char the_next_byte;
  
  spike_net_t *spike;
  lfp_bank_net_t *lfp;

  char tmp_spike[MAX_PACKET_BYTES];

//spike_net_t spike;

  //fread(buff, sizeof(char), 1000, in_f);
  //buffToSpike( spike, buff, false);

  //printf("spike->n_chans: %d\n", spike->n_chans);

  //printf("still ok.\n"); fflush(stdout);

  fread  (buff_head, sizeof(char), 4,  in_f);
  fseek( in_f, -4, SEEK_CUR );

  // Read the header
  the_type = buff_head[0];
  memcpy( &the_length, buff_head+1, 2 );
  memcpy( &the_next_byte, buff_head+3, 1);


  if (the_length != 68 && the_length != 294){
    bad_length_packets++;
    if(!auto_drop_bad_length_packets){
      printf("the_length: %d\n", the_length);
      interactive = true;
      interactive_wait("Bad the_length.\n");
    }

    if(auto_drop_bad_length_packets){
      int search_length = 0;
      while(the_length != 68 && the_length != 294){
	search_length++;
	fseek( in_f, 1, SEEK_CUR );
	fread  (buff_head, sizeof(char), 4,  in_f);
	fseek( in_f, -4, SEEK_CUR );
	the_type = buff_head[0];
	memcpy( &the_length, buff_head+1, 2 );
	memcpy( &the_next_byte, buff_head+3, 1);
      }
      //printf("Succeeded after %d advances\n", search_length);
    }

    fread  (buff_head, sizeof(char), 4,  in_f);
    fseek( in_f, -4, SEEK_CUR );
    the_type = buff_head[0];
    memcpy( &the_length, buff_head+1, 2 );
    memcpy( &the_next_byte, buff_head+3, 1);
    
    //printf("About to leave corruption-handling block.\n");
    //show_header(true);

  }

  if(interactive){
    show_header(true);
    printf("About to fread %d bytes\n", the_length);
  }
  //printf("Wanted sourcetype: %c, buff_type:%c length:%d next_byte:%d\n", 
  //	 sourcetype, the_type, the_length, the_next_byte);

  fread (buff,  sizeof(char),the_length, in_f);

  //printf("still ok after fread.\n");fflush(stdout);

  if ( charToType(the_type) != sourcetype){
    return false;
  }

  if ( charToType(the_type) == NETCOM_UDP_SPIKE ){
    
    spike = (spike_net_t*)arte_packet;
    //printf("ok after spike= assignment\n"); fflush(stdout);

    //if(bad_length_packets > 0)
    //  print_packet(spike, sourcetype);

    buffToSpike( spike, buff, false );
    //printf("ok after buffToSpike.\n"); fflush(stdout);
    ok_packet = ( spike->name == sourcename );
    
    //spike_net_t *this_spike = (spike_net_t *)arte_packet;
    for(int i = 0; i < spike->n_chans * spike->n_samps_per_chan; i++){
      spike->data[i] = spike->data[i] / 16;
    }
    if(spike->ts > (UINT32_MAX - 30000) ){   // is it later than 10 seconds before the end of the valid time range
      ok_packet = false;
      //printf("Found spike with bad_ts:%d  Current spike_count is:%d   Dropping it.\n", spike->ts, spike_count);
    }

    if(spike->ts > low_time_threshold && (!encountered_low)){
      printf("Found a spike with ts:%d which is below the time-theshold:%d while low_encoundered is false.  Dropping it.\n",
             spike->ts, spike_count);
      return false;
    }

    if(false){
      printf("sought-after sourcename:%d sourcetype:%c  found name:%d type:%c\n",
	     sourcename, sourcetype, spike->name, the_type);
    }

    if(false & ok_packet & (sourcetype == NETCOM_UDP_SPIKE)){
      printf("test in get_next_spike: spike->ts:%d, name:%d n_chans:%d\n",
	     spike->ts, spike->name, spike->n_chans);
    }

    if(ok_packet & (sourcetype == NETCOM_UDP_SPIKE) & verbose){
      printf("OK_PACKET!  spikename:%d  sourcename:%d\n", spike->name, sourcename);
      fflush(stdout);
    }
    //printf("ABOUT TO RETURN\n");
    if (ok_packet)
      encountered_low = true;
    return (ok_packet & (sourcetype == NETCOM_UDP_SPIKE)); 
  }
   
  if (sourcetype == NETCOM_UDP_LFP){
    
    lfp = (lfp_bank_net_t *)arte_packet;
    buffToWave( lfp, buff, false );
    ok_packet = ( lfp->name == sourcename );
    for(int i = 0; i < (lfp->n_chans * lfp->n_samps_per_chan); i++){
      lfp->data[i] = lfp->data[i]/16;
    }
    if(lfp->ts > (UINT32_MAX - 10000)){
	ok_packet = false;
	//printf("dropped a bad wave packet.  timestamp was:%d\n", lfp->ts);
    }

    if(false){
    printf("lfp_packet: sought-after sourcename:%d sourcetype:%c  found name:%d type:%c\n",
	   sourcename, sourcetype, lfp->name, the_type);
    }

    return (ok_packet & (sourcetype == NETCOM_UDP_LFP));

  }

}
  
