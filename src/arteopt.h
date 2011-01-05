#ifndef ARTEOPT_H
#define ARTEOPT_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>
#include <vector>
#include "trode.h"
#include <NIDAQmx.h>
#include <map>
#include "filt.h"

extern std::string setup_config_filename;
extern std::string session_config_filename;

extern std::map<std::string, Trode> trode_map;

// struct to represent a daq card for neural data
struct neural_daq{
  int id;
  std::string dev_name;
  int n_samps_per_buffer;
  int n_chans;
  std::string in_filename;
  std::string raw_dump_filename;
  float64 *data_ptr;
};

// map list for daq cards
extern std::map <int, neural_daq> neural_daq_map;

// map list for filter objects
extern std::map <std::string, Filt> filt_map;

// vector of display ports (prob not to include 'main' window)
// but 'tetrode banks' and eeg windows

// std::map <std::string, Gtk/Window> window_map;  DON"T FORGET TO UNCOMMENT THIS WHEN ITS GUI TIME :)

extern boost::property_tree::ptree setup_pt;
extern boost::property_tree::ptree session_pt;

void arte_init(int argc,char *argv[], const std::string &, const std::string &); // argc, argv, setup filename, session filename 

void arte_setup_init(int argc, char *argv[]);
void arte_session_init(int argc, char *argv[]);
void arte_setup_daq_cards();  // eg tasks, virtual chans, callback attachment

int init_new_trode(boost::property_tree::ptree::value_type &v, Trode &new_trode); // ptree_value_type &v copied from recipe, don't quite understand this
neural_daq new_neural_daq(boost::property_tree::ptree::value_type &v);

int save_setup_config(std::string &);
int save_session_config(std::string &);


#endif
