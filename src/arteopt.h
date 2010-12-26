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

void arte_init(std::string &, std::string &); // setup filename, session filename 

int save_setup_config(std::string &);
int save_session_config(std::string &);


#endif
