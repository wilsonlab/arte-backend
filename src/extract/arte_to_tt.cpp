#include "arte_to_tt.h"

FILE *in_f, *out_f;

int main(int argc, char *argv[]){

  spike_net_t spike, first_spike;
  spike_net_t *spike_p;
  uint16_t trodename;
  int ok_spike;
  
  char input_filename[200], output_filename[200];

  init_filenames(argc, argv, input_filename, output_filename, &trodename);

  in_f = fopen(input_filename, "rb");
  out_f = fopen(output_filename, "wb");

  get_next_spike(&first_spike, trodename);

  write_file_header(&first_spike, argc, argv);

  write_spike(&first_spike);

  while( feof(in_f) == 0){
    ok_spike = get_next_spike(&spike, trodename);
    write_spike(&spike);
  }

  printf("finished.\n");
  exit(0);

}

void write_spike(spike_net_t *spike){
  
  fwrite(&(spike->ts), 1, sizeof(spike->ts), out_f);
  fwrite(spike->data, (spike->n_chans * spike->n_samps_per_chan), sizeof(rdata_t),  out_f);
}

void ps(){
  fprintf(out_f,"%");
}

void write_file_header(spike_net_t *spike, int argc, char *argv[]){

  ps(); ps(); fprintf(out_f, "BEGINHEADER\n");
  ps(); fprintf(out_f, " Program\tadextract\n"); //Problem?
ps();   fprintf(out_f, " Program Version:\t1.18\n"); //Problem?
  ps(); fprintf(out_f, " Argc:\t%d\n", argc);
  for(int i = 0; i < argc; i++){
    ps(); fprintf(out_f, " Argv[i+1] :\t%s\n", argv[i]); //Problem?
  }
  ps(); fprintf(out_f, " Date:\tTue Jun 21 09:53:08 2011\n"); //Problem?
 ps();  fprintf(out_f, " Directory:\t/home/greghale/data/joe/062011\n"); //Problem?
  ps(); fprintf(out_f, " Hostname:\tjellyroll\n"); //sloppy
  ps(); fprintf(out_f, " Architecture:\ti686\n"); 
  ps(); fprintf(out_f, " User:\tgreghale ()\n"); //sloppy
  ps(); fprintf(out_f, " File type:\tBinary\n");
  ps(); fprintf(out_f, " Extraction type:\ttetrode waveforms\n");
  ps(); fprintf(out_f, " Probe:\t0\n");  //Problem?
  ps(); fprintf(out_f, " Fields\ttimestamp,%d,%d,%d waveform,%d,%d,%d\n",8,4,1,2,2,
	  spike->n_chans * spike->n_samps_per_chan);
  ps(); fprintf(out_f, "\n");
  ps(); fprintf(out_f, " Beginning of header from input file 'raw/02200320.sgh'\n"); //Problem?
  ps(); fprintf(out_f, " mode: SPIKE\n");
  ps(); fprintf(out_f, " adversion:\t1.36b\n");
  ps(); fprintf(out_f, " rate:\t250000.000000\n"); //Problem?
  ps(); fprintf(out_f, " nelectrodes: 2\n");
  ps(); fprintf(out_f, " nchannels:\t8\n");
  ps(); fprintf(out_f, " nelect_chan:\t%d\n", spike->n_chans); //Problem?
  ps(); fprintf(out_f, " errors:\t%d\n",0); // me?  errors?
  ps(); fprintf(out_f, " disk_errors:\t%d\n",0);
  ps(); fprintf(out_f, " dma_bufsize:\t%d\n",24567); //Problem?
  ps(); fprintf(out_f, " spikelen:\t%d\n", spike->n_samps_per_chan); //Problem?
  ps(); fprintf(out_f, " spikesep:\t%d\n", 32); // sloppy.  Do I need to put refrac period in packets?
  int chan_offset[8] = {65, 122, 180, 238, 296, 353, 411, 469};
  for(int i = 0; i < 8; i++){
    ps(); fprintf(out_f, " channel %d ampgain:\t%d\n", i, 10000); // sloppy
    ps(); fprintf(out_f, " channel %d adgain:\t%d\n", i, 0);
    ps(); fprintf(out_f, " channel %d filter:\t%d\n", i, 136); // somewhat sloppy (does spikeparms care about filter settings?)
    uint16_t this_thresh;
    if(i <= spike->n_chans){
      this_thresh = spike->thresh[i];
    }else{
      this_thresh = 154;
    }
    ps(); fprintf(out_f, " channel %d threshold:\t%d\n", i, this_thresh);
    ps(); fprintf(out_f, " channel %d color:\t%d\n",i, 15);
    ps(); fprintf(out_f, " channel %d offset:\t%d\n", i, chan_offset[i]);
    ps(); fprintf(out_f, " channel %d contscale:\t%d\n", i, 0);
  }
  ps(); fprintf(out_f, " spike_size\t%d\n", 
	  (spike->n_chans * spike->n_samps_per_chan * 2 + 8)); // seemed to be the meaning from a real tt file?  is it important?
  ps(); fprintf(out_f, " fields: int electrode_num; long timestamp; int data[%d]\n",
	  spike->n_chans * spike->n_samps_per_chan);
  ps(); fprintf(out_f, " End of header from input file 'raw/02200320.sgh'\n"); //Problem?
  ps(); fprintf(out_f, "\n");
  ps(); ps(); fprintf(out_f, "ENDHEADER\n");

}

// void tfprintf(FILE *fp, char format_str[], ...){
//   fprintf(fp, "%");
//   fprintf(fp, format_str, ...);
// }

// void ttfprintf(FILE *fp, char format_str[], ...){
//   fprintf(fp, "%%");
//   fprintf(fp, format_str, ...);
// }

void init_filenames(int argc, char *argv[], 
		    char input_filename[], char output_filename[], 
		    uint16_t *trodename){
  
  if(argc != 7){
    printf("Argc was %d\n",argc);
    for(int i = 0; i < argc; i++){
      printf("Argv[%d] was: %s\n", i, argv[i]);
    }
    printf("Normal usage: arte_to_tt -trodename 06 -i input_file.data -o output_file.tt\n");
    exit(1);
  }

  for(int i = 1; i < 5; i++){
    if( strcmp( argv[i], "-i" ) == 0)
      strcpy( input_filename, argv[i+1]);
    if( strcmp( argv[i], "-o") == 0)
      strcpy( output_filename, argv[i+1]);
    if( strcmp( argv[i], "-trodename") == 0){
      sscanf("%d", argv[i], trodename);
    }
  }
}

  bool get_next_spike(spike_net_t* spike, uint16_t trodename){

  char buff_head[100]; // excessively big head
  char buff[4000];
  char the_type;
  uint16_t the_length;
  //spike_net_t spike;

  fread  (buff_head, 4,sizeof(char), in_f);
  fseek( in_f, -4, SEEK_CUR );

  the_type = buff_head[0];
  memcpy( &the_length, buff_head+1, 2 );
  
  fread (buff, the_length, sizeof(char), in_f);

  if( charToType(the_type) == NETCOM_UDP_SPIKE );
  buffToSpike( spike, buff, false );

  return( spike->name == trodename );

}
  
