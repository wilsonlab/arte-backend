#include "process_command.h"
#include "util.h"

extern bool arte_disk_on;

void process_command(command_t *cmd){

  int c_argc;
  char c_argv[MAX_N_TOKENS][MAX_TOKEN_SIZE];

  int trode_names[MAX_N_TOKENS];
  int n_trode_names;
  int lfp_bank_names[MAX_N_TOKENS];
  int n_lfp_bank_names;
  int chan_inds[MAX_N_TOKENS];
  int n_chan_inds;
  
  parse_command(cmd->command_str, &c_argc, c_argv);
  
  if(false){
    for(int i = 0; i < c_argc; i++){
      printf("c_argv[%d]=%s\n",i,c_argv[i]);
    }
  }

  build_num_lists(c_argc, c_argv, 
		  trode_names, &n_trode_names, 
		  lfp_bank_names, &n_lfp_bank_names,
		  chan_inds, &n_chan_inds);

  if( strcmp( "setthreshold", c_argv[0] ) == 0){
    printf("Name match to setthreshold\n");
    float thresh;
    sscanf(c_argv[1], "%f", &thresh);
    double d_thresh = double(thresh);
    printf("thresh before fn: %f  thresh after fn: %d\n", d_thresh, ftor(&d_thresh));
    set_threshold( trode_names, n_trode_names, chan_inds, n_chan_inds, ftor(&d_thresh) );
  }

  if( strcmp( "setpostsamps", c_argv[0] ) == 0){
    int samps;
    sscanf(c_argv[1], "%d", &samps);
    set_postsamps( trode_names, n_trode_names, samps );
  }

  if( strcmp( "resetclock", c_argv[0] ) == 0){
    reset_clock();
  }

  if( strcmp( "diskclock", c_argv[0] ) == 0 ){
    bool d = arte_disk_on;
    printf("CAUGHT IT!\n");
    if(d){
      arte_disk_on = 0;
      printf("Stopping disk writing.\n");
      //reset_clock();
    } else {
      reset_clock();
      arte_disk_on = 1;
      printf("Starting disk writing.\n");
    }
    fflush(stdout);
  }

  if( strcmp( "disk", c_argv[0] ) == 0){
    bool d = arte_disk_on;
    if(d){
      arte_disk_on = 0;
      printf("Stopping disk writing.\n");
    } else {
      arte_disk_on = 1;
      printf("Starting disk writing.\n");
    }
  }

  if( strcmp( "stopacquisition", c_argv[0] ) == 0){
    neural_daq_stop_all();
  }

}

void reset_clock(){
  printf("Resetting the clock. (Note, you'll want to do this about\n");
  printf("a half-second before the clock signal comes back online.\n");
  printf("If AD-slave, reset here just a moment after resetting master clock.\n");
  fflush(stdout);
  arte_timer.reset();

}

void set_threshold(int *trode_names, int n_trode_names, int *chan_inds, int n_chan_inds, rdata_t new_threshold){
  printf("in set_threshold. trode_name[0] is %d. n_trode_names is %d\n", trode_names[0], n_trode_names);
  for(int i = 0; i < n_trode_names; i++){
    int this_ind = ind_from_trode_name( trode_names[i] );
    printf("this_ind is %d\n", this_ind);
    if (this_ind > -1){
      for(int t = 0; t < n_chan_inds; t++){
	trode_array[this_ind].thresholds[ chan_inds[t] ] = new_threshold;
	printf("Just tried to set trode_array[%d].thresholds[%d] = %d\n", this_ind, chan_inds[t], new_threshold);
      }
      trode_array[this_ind].setup_spike_array();
    }
  }
}

void set_postsamps(int *trode_names, int n_trode_names, int new_postsamps){
  for(int i = 0; i < n_trode_names; i++){
    int this_ind = ind_from_trode_name( trode_names[i] );
    if(this_ind > -1){
      printf("trying to set trode[%d].samps_after_trig to %d\n",this_ind, new_postsamps);
      trode_array[this_ind].samps_after_trig = new_postsamps;
      trode_array[this_ind].n_samps_per_spike = 
	trode_array[this_ind].samps_before_trig + new_postsamps + 1;
      trode_array[this_ind].setup_spike_array();
    }
  }
}

int ind_from_trode_name(uint16_t name){
  for(int i = 0; i < n_trodes; i++){
    if( trode_array[i].name == name ){
      return i;
    }
  }
  printf("Could not find a trode with name %d\n", name);
}

int ind_from_lfp_bank_name(uint16_t name){
  for(int i = 0; i < n_lfp_banks; i++){
    if( lfp_bank_array[i].lfp_bank_name == name ){
      return i;
    }
  }
  printf("Could not find an lfp_bank with name %d\n", name);
}

void build_num_lists(int c_argc, char c_argv[MAX_N_TOKENS][MAX_TOKEN_SIZE],
		     int trode_names[MAX_N_TOKENS], int *n_trode_names,
		     int lfp_bank_names[MAX_N_TOKENS], int *n_lfp_bank_names,
		     int chan_inds[MAX_N_TOKENS], int *n_chan_inds){
  
  
  for(int i = 0; i < c_argc; i++){
    int start_ind;
    int stop_ind = -1;
    char this_first_char;
    
    if (strcmp(c_argv[i], "-trodenames")==0){
      int j = i+1;
      stop_ind = -1;
      start_ind = j;
      while(stop_ind == -1){
	if( (c_argv[j][0] == '-') | (j >= c_argc) )
	  stop_ind = j - 1;
	j++;
      }
      num_list_from_c_argv(c_argc, c_argv, start_ind, stop_ind, trode_names);
      if(stop_ind >= start_ind)
	*n_trode_names = stop_ind - start_ind + 1;
    }
    if (strcmp(c_argv[i], "-lfpbanknames")==0){
      int j = i+1;
      stop_ind = -1;
      start_ind = j;
      while(stop_ind == -1){
	if( (c_argv[j][0] == '-') | (j >= c_argc) )
	  stop_ind = j - 1;
	j++;
      }
      num_list_from_c_argv(c_argc, c_argv, start_ind, stop_ind, lfp_bank_names);
      if(stop_ind >= start_ind)
	*n_lfp_bank_names = (stop_ind - start_ind + 1);
    }
    if (strcmp(c_argv[i], "-chaninds")==0){
      int j = i+1;
      stop_ind = -1;
      start_ind = j;
      while(stop_ind == -1){
	if( (c_argv[j][0] == '-') | (j >= c_argc) )
	  stop_ind = j - 1;
	j++;
      }
      num_list_from_c_argv(c_argc, c_argv, start_ind, stop_ind, chan_inds);
      if(stop_ind >= start_ind)
	*n_chan_inds = (stop_ind - start_ind + 1);
    }
  }

  if(false){
    printf("trodenames[%d]: ", *n_trode_names);
    for(int i = 0; i < *n_trode_names; i++)
      printf("%d ", trode_names[i]);
    printf("\nlfpbanknames[%d]: ", *n_lfp_bank_names);
    for(int i = 0; i < *n_lfp_bank_names; i++)
      printf("%d ", lfp_bank_names[i]);
    printf("\nchaninds[%d]: ", *n_chan_inds);
    for(int i = 0; i < *n_chan_inds; i++)
      printf("%d ", chan_inds[i]);
    printf("\n");
  }

}

void num_list_from_c_argv(int c_argc, char c_argv[MAX_N_TOKENS][MAX_TOKEN_SIZE],
			  int start_ind, int stop_ind, int* num_list){

  printf("start_ind:%d stop_ind:%d\n",start_ind,stop_ind);

  int n_elem = stop_ind - start_ind + 1;
  for(int i = 0; i < n_elem; i++){
    sscanf(c_argv[i+start_ind], "%d", num_list+i);
  
    printf("c_argv[%d] was %s.  Now num_list[%d] is %d\n", i+start_ind, c_argv[i+start_ind],i,num_list[i]); 
  }
}

void parse_command(char *cmd_str, int *c_argc, char c_argv[MAX_N_TOKENS][MAX_TOKEN_SIZE]){
  int t = 0;
  int tc = 0;
  char this_char;
  memset( c_argv, 0, MAX_N_TOKENS * MAX_TOKEN_SIZE );
  printf("the command:%s\n",cmd_str);
  for(int i = 0; i < MAX_N_TOKENS * MAX_TOKEN_SIZE; i++){
    this_char = cmd_str[i];
    if(this_char == '\0'){
      *c_argc = t + 1;
      break;
    }
    if(this_char == ' '){
      // end this token
      c_argv[t][tc] = '\0';
      t++;
      tc = 0;
      continue;
    }
    c_argv[t][tc] = this_char;
    tc++;
    //printf("c_argv[%d][%d] = %c\n",t,tc,this_char);
  }
}
