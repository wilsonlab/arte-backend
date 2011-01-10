#include "filt.h"
#include <boost/property_tree/exceptions.hpp>
#include "util.h"
#include <string>

// wrap coordinate for circular buffer
int rel_pt(int pos, int curs, int buf_len){
  return ( (curs+pos) % buf_len );
}

void filter_data(float64 *in_buf, Filt filt, int *chans, int n_chans, int in_buf_len, 
		 int out_buf_len, int *u_curs_p, int *f_curs_p, int *ff_curs_p,  float64 *u_buf, float64 *f_buf, float64 *ff_buf){

  int u_curs = *u_curs_p;
  int f_curs = *f_curs_p;
  int ff_curs = *ff_curs_p;
  int out_pt; // an index to use in the for loop.
  int in_pt, in_pt_c, in_pt_h1, in_pt_h2;  
  int h;
  float64 *a, *b;
  float64 value;

  if(u_curs == out_buf_len)
    u_curs = 0;
  
  // first copy data into unfiltered buffer and initialize the corresponding out buffers
  for(int n = 0; n < in_buf_len; n++){
    for(int c = 0; c < n_chans; c++){
      in_pt = chans[c]*n_chans + n;
      out_pt = c * n_chans + n + u_curs;
      h = c * n_chans + n + u_curs;
      value = in_buf[in_pt];
      u_buf[in_pt] = value;
      u_buf[c*n_chans + n + u_curs] = in_buf[chans[c]*n_chans + n];                           // don't need rel_pt here.
      f_buf[c*n_chans + n + u_curs] = 0.0;
      ff_buf[c*n_chans + n + ff_curs] = 0.0;                                         // CHECK these indices. When do cursor positions get updated?
    }
  }

  if(filt.filt_num_sos == 1){
    // then we most likely have an FIR.  This code will take care of FIR and 
    // single-segment IIR filters, too, as long as the first last denom
    // coefficient is set to 0.
    for(int n = 0; n < in_buf_len; n++){                                           // process in_buf_len points
      out_pt = n + u_curs;
      for(int p = 0; p <= filt.order; p++){                                        // iterate over history points (we need order of them)
	h = -1*filt.order + p;                                 // history index should be -order, -order+1, ..., 0
	in_pt = rel_pt(h, n+u_curs, out_buf_len);
	for(int c = 0; c < n_chans; c++){
	  f_buf[c*n_chans + out_pt] += 
	    ( in_buf[c*n_chans + in_pt]*filt.num_coefs[p]*filt.input_gains[0] -    // feedforward path
	      f_buf[c*n_chans + in_pt]*filt.denom_coefs[p]);                       // feedback path
	}                                                                          // 
      }
    }                                                                              // viola.  Check the sign conventions. 
  } else {
    // then we have biquad sections, and can dispense with the for loop over p
    // b/c we know the fixed history length.
    int n_segs = filt.filt_num_sos;
    for(int s = 0; s < n_segs; s++){
      if(s > 0){
	for(int n = 0; n < in_buf_len; n++){                                         // copy f_buf into u_buf if this is any run of the filter but the first.
	  for(int c = 0; c < n_chans; c++){                                          // so that the second section gets the output of the first as its 'raw'
	    value = f_buf[c*n_chans + n];                                          // so that the second section gets the output of the first as its 'raw'
	    u_buf[c*n_chans + n] = value;
	  }
	}
      } 
      for(int n = 0; n < in_buf_len; n++){                                                     //the
	out_pt = n+u_curs;
	in_pt_c = n+u_curs;
	in_pt_h1 = rel_pt(-1,n+u_curs, out_buf_len);
	in_pt_h2 = rel_pt(-2,n+u_curs, out_buf_len);
	for(int c = 0; c < n_chans; c++){
	  f_buf[c*n_chans + out_pt] =
	    ( u_buf[c*n_chans + in_pt_c]*b[s*n_segs + 0]*filt.input_gains[s] +
	      u_buf[c*n_chans + in_pt_h1]*b[s*n_segs + 1]*filt.input_gains[s] +
	      u_buf[c*n_chans + in_pt_h2]*b[s*n_segs + 2]*filt.input_gains[s] -
	      f_buf[c*n_chans + in_pt_h1]*a[s*n_segs + 1] -
	      f_buf[c*n_chans + in_pt_h2]*a[s*n_segs + 2] );  
	} // end loop over chans
      } //end loop over points in this input chuckn
    }  // end loop over segs
  } // end if iir

  // Still need to do the filtfilt work

}

Filt::Filt(){
}

Filt::~Filt(){
}

void Filt::init(boost::property_tree::ptree &filt_pt){
  int n_coefs;
  filt_name = filt_pt.data();

  assign_property<std::string>("type", &type, filt_pt, filt_pt, 1);
  assign_property<int>("order", &order, filt_pt, filt_pt, 1);
  assign_property<bool>("filtfilt", &filtfilt, filt_pt, filt_pt, 1);
  assign_property<int> ("filtfilt_wait_n_buffers", &filtfilt_wait_n_buffers, filt_pt, filt_pt, 1);

  data_cursor = 0;

  if(type.compare("fir") == 0){
    n_coefs = order;
    num_coefs = new float64 [order];
    denom_coefs = new float64 [order];
    filt_num_sos = 1; // second-order-sections is the wrong term here; we mean one plain-old-section (full filter order)
  }
  else if( (type.compare("iir") == 0) ){
    filt_num_sos = order / 2;
    n_coefs = order * 3;
    num_coefs = new float64 [n_coefs];
    denom_coefs = new float64 [n_coefs];
  } else{
    std::cerr << "Can't use filt type: " << type << std::endl;
  }
  assign_property<float64>("num_coefs", num_coefs, filt_pt, filt_pt,n_coefs);
  assign_property<float64>("denom_coefs", denom_coefs, filt_pt, filt_pt, n_coefs); 
}
