#include "plot_waveforms.h"
#include "datapacket.h"

#define MAX_N_ROWS 200

extern uint32_t n_spikes_caught;

void plot_waveforms(spike_net_t *spike, int max_y, int n_rows, int row_for_zero_y){

  int row_min_y[MAX_N_ROWS];
  int row_max_y[MAX_N_ROWS];

  int y_per_row = (max_y - 0) / (n_rows - row_for_zero_y);

  int n,r,c,s,y;
  for(n = n_rows-1; n >= 0; n--){
    row_max_y[n] = max_y - y_per_row*(n_rows - (n));
    row_min_y[n] = max_y - y_per_row*(n_rows - (n-1));
  }

  int n_disp_cols = spike->n_chans*spike->n_samps_per_chan + spike->n_chans + 1;
  
  for(n = 0; n < n_disp_cols; n++)
    printf("-");
  printf("\n");

  for(r = n_rows -1; r >= 0; r--){
    
    for(c = 0; c < spike->n_chans; c++){
      printf("|");
      for(s = 0; s < spike->n_samps_per_chan; s++){
   	y = spike->data[s*spike->n_chans + c];
   	if( (y <= row_max_y[r]) && (y > row_min_y[r]) ){
	//	if(( y >= row_max_y[r])){
   	  printf("o");
   	} else {
   	  printf(" ");
   	}
      }
    }
    printf("| \n");
    //printf("| %d : %d\n", row_min_y[r], row_max_y[r]);
  }
  
  for(n = 0; n < n_disp_cols; n++){
    printf("-");
  }
  printf("\nSeq_diff: %d  Ts: %f  ArtE_count: %d  Our count: %d\n",
	 (spike->seq_num+1) - n_spikes_caught,
	 (float)spike->ts/10000,
	 spike->seq_num + 1,
	 n_spikes_caught);

}
