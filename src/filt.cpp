#include "filt.h"
#include <boost/property_tree/exceptions.hpp>
#include "util.h"
#include <string>

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
