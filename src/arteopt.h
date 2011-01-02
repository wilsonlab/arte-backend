#ifndef ARTEOPT_H
#define ARTEOPT_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>
#include <vector>
#include "trode.h"
#include <NIDAQmx.h>
#include <map>

extern std::string setup_config_filename;
extern std::string session_config_filename;

// map of tetrode objects { string name, trode object }
// the syntax here is very wrong - need to read about std::map
// key will be name string; data will be Trode object
// Why map, not vector or array?  b/c tetrodes aren't inherently ordered
// During initialization, we may need to refer to trodes by name (for assigning trodes into windows)
// But will lookup be slow?  It will only happen during init.
// During init, each trode will hold a pointer into the buffer that gets filled by the daq card
// rather than the daq card having to look up the trode repeatedly on each buffer cycle

extern Trode test_t;

extern std::map<std::string, Trode> trode_map;

// struct to represent a daq card for neural data
struct neural_daq{
  int id;
  std::string dev_name;
  int n_samps_per_buffer;
  int n_chans;
  float64 *data_ptr;
};

// map list for daq cards
extern std::map <int, neural_daq> neural_daq_map;

// vector of display ports (prob not to include 'main' window)
// but 'tetrode banks' and eeg windows

// std::map <std::string, Gtk/Window> window_map;  DON"T FORGET TO UNCOMMENT THIS WHEN ITS GUI TIME :)

extern boost::property_tree::ptree setup_pt;
extern boost::property_tree::ptree session_pt;

void arte_init(int argc,char *argv[], const std::string &, const std::string &); // argc, argv, setup filename, session filename 

void arte_setup_init(int argc, char *argv[]);
void arte_session_init(int argc, char *argv[]);
void arte_setup_daq_cards();  // eg tasks, virtual chans, callback attachment

Trode new_trode(boost::property_tree::ptree::value_type &v); // ptree_value_type &v copied from recipe, don't quite understand this
neural_daq new_neural_daq(boost::property_tree::ptree::value_type &v);

int save_setup_config(std::string &);
int save_session_config(std::string &);


#endif
