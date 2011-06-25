#include "plot_waveforms.h"
#include "../netcom/datapacket.h"

#define MAX_N_ROWS 200

void plot_waveforms(spike_net_t *spike, int max_y, int n_rows, int row_for_zero_y){

  int row_min_y[MAX_N_ROWS];
  int row_max_y[MAX_N_ROWS];

  int y_per_row = (max_y - 0) / (n_rows - row_for_zero_y);

  int n,r,c,s,y;
  for(n = n_rows-1; n >= 0; n++){
    row_max_y[n] = max_y - y_per_row*(n_rows - (n-1));
    row_min_y[n] = max_y - y_per_row*(n_rows - n);
  }

  int n_disp_cols = spike->n_chans*spike->n_samps_per_chan + spike->n_chans + 1;
  
  for(n = 0; n < n_disp_cols; n++)
    printf("-");
  printf("\n");

  for(r = 0; r < n_rows; r++){
    
    for(c = 0; c < spike->n_chans; c++){
      printf("|");
      for(s = 0; s < spike->n_samps_per_chan; s++){
	y = spike->data[s*spike->n_chans + c];
	if( (y <= row_max_y[r]) && (y > row_min_y[r]) ){
	  printf("*");
	} else {
	  printf(" ");
	}
      }
    }
    printf("|\n");
  }

  for(n = 0; n < n_disp_cols; n++){
    printf("-");
  }

}
