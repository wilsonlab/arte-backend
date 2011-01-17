#include "filt.h"
#include <boost/property_tree/exceptions.hpp>
#include "util.h"
#include <string>

#define CBUF(X,L) (L+X)%L // assumes X >= -L

// wrap coordinate for circular buffer
int rel_pt(int pos, int curs, int buf_len){
  int r = (( curs + pos) % buf_len);
  if (r < 0)                    // funny situation - our implementation of c returns mod
    r += buf_len;               // with the same sign of the numerator.  tricky! 
  return r;
}

void filter_data(float64 *in_buf, Filt *filt, int *chans, int n_chans, int in_buf_len, 
		 int out_buf_len, int *u_curs_p, int *f_curs_p, int *ff_curs_p,  float64 *u_buf, float64 *f_buf, float64 *ff_buf){

  int u_curs = *u_curs_p;
  int f_curs = *f_curs_p;
  int ff_curs = *ff_curs_p;
  int out_pt; // an index to use in the for loop.
  int n_segs;
  int in_pt, in_pt_c, in_pt_h1, in_pt_h2;  
  int h,n,c,p,s;
  h = n = c = p = s = 0;
  float64 *a, *b;
  float64 value;

  a = filt->denom_coefs;
  b = filt->num_coefs;

  //  if(u_curs == out_buf_len)
  // u_curs = 0;

  // up is just a loop to repeat the filtering multiple times, for testing CPU usage
  for(int up = 0; up < 1; up++){

  // first copy data into unfiltered buffer and initialize the corresponding out buffers

   for(n = 0; n < in_buf_len; n++){
     for(c = 0; c < n_chans; c++){
       //std::cout << "*********** In buf: " << in_buf[0] << "\r" ;
       in_pt = chans[c]*in_buf_len + n;
       out_pt = c * out_buf_len + n + u_curs;  
       //u_buf[out_pt] = in_buf[in_pt];
        u_buf[c*out_buf_len + n + u_curs] = in_buf[chans[c]*in_buf_len + n];                           // don't need rel_pt here.

     }
   }
   if(filt->filt_num_sos == 1){
     // then we most likely have an FIR.  This code will take care of FIR and 
     // single-segment IIR filters, too, as long as the first last denom
     // coefficient is set to 0.
     for(n = 0; n < in_buf_len; n++){                                           // process in_buf_len points
       out_pt = n + u_curs;
       for(p = 0; p <= filt->order; p++){                                        // iterate over history points (we need order of them)
	 h = -1*filt->order + p;                                 // history index should be -order, -order+1, ..., 0
	 in_pt = rel_pt(h, n+u_curs, in_buf_len);
	 for(c = 0; c < n_chans; c++){
	   f_buf[c*out_buf_len + out_pt] += 
	     ( in_buf[c*in_buf_len + in_pt]*filt->num_coefs[p]*filt->input_gains[0] -    // feedforward path
	       f_buf[c*out_buf_len + in_pt]*filt->denom_coefs[p]);                       // feedback path
	 } // end loop over chans chan                                                                         // 
       } // end loop over history points
     } //end loop over point in out_buf to compute                                       // viola.  Check the sign conventions. 
   } // end if block for sos == 1
   else {
     // then we have biquad sections, and can dispense with the for loop over p
     // b/c we know the fixed history length.
     n_segs = filt->filt_num_sos;
     for(s = 0; s < n_segs; s++){
       
        if(s > 0){
 	 for(n = u_curs; n < u_curs + in_buf_len; n++){                                         // copy f_buf into u_buf if this is any run of the filter but the first.
 	   for(c = 0; c < n_chans; c++){                                          // so that the second section gets the output of the first as its 'raw'
 	     //value = f_buf[c*out_buf_len + n];
 	                                                                              // so that the second section gets the output of the first as its 'raw'
 	     u_buf[c*out_buf_len + n] = f_buf[c*out_buf_len + n];
 	         
	     
 	   }
 	 }
        }
       //    if(s > 0)
       // memcpy(u_buf, f_buf, filt->out_buf_size_bytes);
 
       for(n = 0; n < in_buf_len; n++){                                                     //the
	 out_pt = n+u_curs;
	 in_pt_c = n+u_curs;
	 in_pt_h1 = rel_pt(-1,n+u_curs, out_buf_len);
	 in_pt_h2 = rel_pt(-2,n+u_curs, out_buf_len);
	 for(c = 0; c < n_chans; c++){
	   
 	   f_buf[c*out_buf_len + out_pt] =
 	     ( u_buf[c*out_buf_len + in_pt_c]*b[s*3 + 0]*filt->input_gains[s] +
 	       u_buf[c*out_buf_len + in_pt_h1]*b[s*3 + 1]*filt->input_gains[s] +
 	       u_buf[c*out_buf_len + in_pt_h2]*b[s* + 2]*filt->input_gains[s] -
 	       f_buf[c*n_chans + in_pt_h1]*a[s*3 + 1] -
 	       f_buf[c*n_chans + in_pt_h2]*a[s*3 + 2] );  
	   //f_buf[c*out_buf_len + out_pt] = -100.0;  
	 } // end loop over chans
       } //end loop over points in this input chuckn
     }  // end loop over segs
   } // end if iir
  } // end 'up' loop

   std::cout << "Begin check." << std::endl;
   std::cout << "Before change, *u_curs_p is " << *u_curs_p << std::endl;
   std::cout << "in_buf_len is " << in_buf_len << std::endl;
   std::cout << "out_buf_len is " << out_buf_len << std::endl;

  (*u_curs_p) = in_buf_len + (*u_curs_p);
  (*f_curs_p) += in_buf_len;
  std::cout << "Before wrap check, *u_curs_p is " << *u_curs_p << std::endl;
  //if( u_curs > out_buf_len || f_curs > out_buf_len){
  //  std::cout << "Strange: u_curs or f_curs is greater than it's supposed to be allowed to get." << std::endl;
  //}
  if((*u_curs_p) == out_buf_len){
    (*u_curs_p) = 0;
    std::cout << "Reset u_curs_p." << std::endl;
  }
  if((*f_curs_p) == out_buf_len)
    (*f_curs_p) = 0;

  std::cout << "After all checking, *u_curs_p is " << (*u_curs_p) << std::endl;
  fflush(stdout);
  
  // Still need to do the filtfilt work
  if((filt->count)++ == 4)
    exit(1);
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
    input_gains = new float64 [1];
    filt_num_sos = 1; // second-order-sections is the wrong term here; we mean one plain-old-section (full filter order)
  }
  else if( (type.compare("iir") == 0) ){
    filt_num_sos = order / 2;
    n_coefs = filt_num_sos * 3;
    num_coefs = new float64 [n_coefs];
    denom_coefs = new float64 [n_coefs];
    input_gains = new float64 [filt_num_sos];
  } else{
    std::cerr << "Can't use filt type: " << type << std::endl;
  }
  assign_property<float64>("num_coefs", num_coefs, filt_pt, filt_pt,n_coefs);
  assign_property<float64>("denom_coefs", denom_coefs, filt_pt, filt_pt, n_coefs);
  if(type.compare("fir") == 0) 
    assign_property<float64>("input_gains", input_gains, filt_pt, filt_pt, 1);
  else
    assign_property<float64>("input_gains", input_gains, filt_pt, filt_pt, filt_num_sos);
  count = 0; // temporary thing for debugging
}

void print_array(float64 *buf, int n_chans, int buf_len, int curs)
{
  std::cout.precision(2);
  for (int c = 0; c < n_chans; c++){
    std::cout << "c" << c << " ";
    for (int p = 0; p < buf_len; p++){
      if(p == curs)
	std::cout << " : ";
      std::cout << buf[c*buf_len + p] << " ";
    }
    std::cout << std::endl;
  }
}

