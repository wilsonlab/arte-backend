#infned ARTEOPT_H_
#define ARTEOPT_H_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>
#include <vector>
#include "trode.h"

string setup_config_filename;
string session_config_filename;

// vector of tetrode objects
std::vector <Trode> trode_list;

// vector of display ports (prob not to include 'main' window)
// but 'tetrode banks' and eeg windows
std::vector <Gtk/Window> window_list;

boost::property_tree::ptree setup_pt;
boost::property_tree::ptree session_pt;

void arte_init(int argc,char *argv[], std::string &, std::string &); // argc, argv, setup filename, session filename 

void arte_setup_init(int argc, char *argv[]);
void arte_session_init(int argc, char *argv[]);
void arte_setup_daq_cards();

void new_trode(ptree::value_type &v);

int save_setup_config(std::string &);
int save_session_config(std::string &);


#endif
