#include "filter_fun.h"

void filter_buffer( Filtered_buffer * fb ){

  int c_in, c_out, samp;  // current sample, channel
  int in_pt, in_pt_c, in_pt_h1, in_pt_h2; // current input point, current, history1, history2
  int pt_h0, pt_h1, pt_h2;
  int out_pt;
  int n_in_chans, n_in_samps; // copy values from Filt_buf to here for speed
  int n_out_chans, n_out_samps;  // copy from fb for speed
  int this_in, this_out;
  int p; // which second-order-section are we on?
  int c;
  int i;
  int i_offset;
  int u_curs, f_curs, ff_curs;

  double *a; // denominator coefficients
  double *b; // numerator   coefficients

  rdata_t * daq_buf = *(fb->ptr_to_raw_stream); // daq's buffer
  rdata_t * u_buf  = fb->u_buf;                 // unfilt buffer
  rdata_t * f_buf  = fb->f_buf;                 // filt buffer
  rdata_t * ff_buf = fb->ff_buf;                // filtfilt buffer
  rdata_t * i_buf  = fb->i_buf;                 // the actual long buffer
  double  * d_buf  = fb->d_buf;
  int *i_ind  = fb->i_ind;

  // copy data into local vars
  n_in_chans =  fb->stream_n_chans;
  n_in_samps =  fb->stream_n_samps_per_chan;
  n_out_chans = fb->n_chans;
  n_out_samps = fb->buf_len;

  a = fb->my_filt.denom_coefs;
  b = fb->my_filt.num_coefs;

  ((fb->u_curs)) += n_in_samps;
  ((fb->f_curs)) += n_in_samps;
  // and reset them to the beginning if it's time to do so.
  if( (fb->u_curs) >= n_out_samps ){
    // reset cursor samp
    ((fb->u_curs)) = 0;
    ((fb->f_curs)) = 0;
  }

  u_curs = fb->u_curs;
  f_curs = fb->f_curs;
  ff_curs= fb->ff_curs;

  // first copy data from in_stream to u_buf
  for(c_out = 0; c_out < n_out_chans; c_out++){
    c_in = fb->channels[c_out];
    for(samp = 0; samp < n_in_samps; samp++){
      this_out = ((samp+u_curs) * n_out_chans) + c_out; // (samp num * n_chans) + chan_num
      this_in =  (samp * n_in_chans)  + c_in;  // (samp num * 
      d_buf[i_ind[0]+this_out] = daq_buf[this_in];
      i_buf[i_ind[0]+this_out] = daq_buf[this_in];
      u_buf[this_out] = (rdata_t) (daq_buf[this_in]);
    }
  }
  
  if((fb->my_filt.filt_num_sos == 1) && false){
    // This is the place where FIR filtering will be implemented
    std::cout << "Tell Greg to implement the fir filter." << std::endl;
    exit(EXIT_FAILURE);
  }

  for(i = 0; i < fb->my_filt.filt_num_sos; i++){
    
    //i_offset = n_out_chans * n_out_samps * p;
    // this is the i'th filtering step
    // first compute f_buf

//     std::cout << "About to filter." << std::endl;
//     fflush(stdout);
    
    for(samp = u_curs; samp < u_curs + n_in_samps; samp++){

//       out_pt =  samp * n_out_chans;
//       in_pt_c = samp * n_out_chans;
//       in_pt_h1 = CBUF( (-1 + samp), n_out_samps  ) * n_out_chans;
//       in_pt_h2 = CBUF( (-2 + samp), n_out_samps  ) * n_out_chans;

      pt_h0 = samp * n_out_chans;
      pt_h1 = CBUF( (samp-1), n_out_samps ) * n_out_chans;
      pt_h2 = CBUF( (samp-2), n_out_samps ) * n_out_chans;

      for( c = 0; c < n_out_chans; c++){

// 	f_buf[ out_pt + c ] =
// 	  (rdata_t) ( u_buf[in_pt_c+c]* b[i*3+0] * fb->my_filt.input_gains[i] +
// 		      u_buf[in_pt_h1+c]*b[i*3+1] * fb->my_filt.input_gains[i] +
// 		      u_buf[in_pt_h2+c]*b[i*3+2] * fb->my_filt.input_gains[i] -
// 		      f_buf[in_pt_h1+c]*a[i*3+1] -
// 		      f_buf[in_pt_h2+c]*a[i*3+2] );

	d_buf[i_ind[i+1] + pt_h0 + c] =
	  ( d_buf[i_ind[i]+pt_h0+c]   * b[i*3+0] * fb->my_filt.input_gains[i] +
	    d_buf[i_ind[i]+pt_h1+c]   * b[i*3+1] * fb->my_filt.input_gains[i] +
	    d_buf[i_ind[i]+pt_h2+c]   * b[i*3+2] * fb->my_filt.input_gains[i] -
	    d_buf[i_ind[i+1]+pt_h1+c] * a[i*3+1] -
	    d_buf[i_ind[i+1]+pt_h2+c] * a[i*3+2] );

	i_buf[i_ind[i+1] + pt_h0 + c] = (rdata_t) d_buf[i_ind[i+1] + pt_h0 + c];

	// copy values into f_buf
	if(i+1 == fb->my_filt.filt_num_sos)
	  f_buf[pt_h0 + c] = i_buf[i_ind[i+1] + pt_h0 + c];

	// pause for debugger
	
	// test if the index hit is right
	//i_buf[ i_ind[i+1] + pt_h0 + c] = 100;
	// NB: it worked - I got all 12's
	if(false){

	  std::cout << 
	    " i_buf[  i_ind[i+1]  +  pt_h0  +  c ] =" << std::endl <<
	    " i_buf[  " << i_ind[i+1] << " + " << pt_h0 << " + " << c << " ] =" << std::endl << std::endl <<
	    "  i_buf[ i_ind[i]+pt_h0+c] * b[i*3 + 0] + input_gains[i] " << std::endl <<
	    "  i_buf[ " << i_ind[i] << "+" << pt_h0 << "+" << c << "] * b[" << i*3+0 << "] * gains[" << i << "]" << std::endl <<
	    " " << i_buf[i_ind[i] + pt_h0 + c] << " * " << b[i*3 + 0] << " * " << fb->my_filt.input_gains[i] << std::endl;
	}
	
	if(false){
	  printf("d_buf[%d] = %f   i_buf[%d] = %d   f_buf[%d] = %d\n", 
		 i_ind[i+1]+pt_h0+c, d_buf[i_ind[i+1]+pt_h0+c],
		 i_ind[i+1]+pt_h0+c, i_buf[i_ind[i+1]+pt_h0+c],
		 pt_h0+c, f_buf[pt_h0+c]);
	}

      }

    }

    
//     // then copy f_buf back into u_buf to prep for the next run
//     for(samp = u_curs; samp < u_curs + n_in_samps; samp++){
//       out_pt =  samp * n_out_chans;
//       in_pt_c = samp * n_out_chans;
//       for( c = 0; c < n_out_chans; c++){
// 	u_buf[ out_pt+c ] = f_buf[ in_pt_c + c ];
//       }
//     }

  }
  // now the loop over sos'es has ended.  f_buf should be ready.


}
