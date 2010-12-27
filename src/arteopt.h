#infned ARTEOPT_H_
#define ARTEOPT_H_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>
#include <vector>
#include "trode.h"
#include <NIDAQmx.h>

string setup_config_filename;
string session_config_filename;

// map of tetrode objects { string name, trode object }
// the syntax here is very wrong - need to read about std::map
// key will be name string; data will be Trode object
// Why map, not vector or array?  b/c tetrodes aren't inherently ordered
// During initialization, we may need to refer to trodes by name (for assigning trodes into windows)
// But will lookup be slow?  It will only happen during init.
// During init, each trode will hold a pointer into the buffer that gets filled by the daq card
// rather than the daq card having to look up the trode repeatedly on each buffer cycle
std::map <string, Trode> trode_map;

// struct to represent a daq card for neural data
struct daq_card{
  string dev_name;
  int n_samps_per_buffer;
  int n_chans;
  float64 *data_ptr;
};

// map list for daq cards
std::map <string, daq_card> daq_card_map;

// vector of display ports (prob not to include 'main' window)
// but 'tetrode banks' and eeg windows
std::map <string, Gtk/Window> window_list;

boost::property_tree::ptree setup_pt;
boost::property_tree::ptree session_pt;

void arte_init(int argc,char *argv[], std::string &, std::string &); // argc, argv, setup filename, session filename 

void arte_setup_init(int argc, char *argv[]);
void arte_session_init(int argc, char *argv[]);
void arte_setup_daq_cards();  // eg tasks, virtual chans, callback attachment

void new_trode(ptree::value_type &v); // ptree_value_type &v copied from recipe, don't quite understand this

int save_setup_config(std::string &);
int save_session_config(std::string &);


#endif
