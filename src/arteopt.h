#ifndef ARTEOPT_H_
#define ARTEOPT_H_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>
#include <vector>
#include "trode.h"
#include <NIDAQmx.h>
#include <map>
#include "filt.h"
#include "neural_daq.h"
#include "filtered_buffer.h"
#include "lfp_bank.h"
#include "netcom.h"
#include <time.h>

extern FILE *main_file;
extern pthread_mutex_t main_file_mutex;

extern uint32_t timestamp;

//extern NetCom command_netcom;
//NetComDat command_netcom_dat;

extern Timer arte_timer;
extern bool timer_is_toy;
//extern uint32_t buffer_count;

extern std::string setup_config_filename;
extern std::string session_config_filename;

extern std::map <std::string, Filt> filt_map;
extern std::map <int, neural_daq> neural_daq_map;

extern boost::property_tree::ptree setup_pt;
extern boost::property_tree::ptree session_pt;

void arte_init(int argc,char *argv[], const std::string &, const std::string &); // argc, argv, setup filename, session filename 

void arte_setup_init(int argc, char *argv[]);
void arte_session_init(int argc, char *argv[]);
void arte_network_init(int argc, char *argv[]);
//int  arte_setup_daq_cards();  // eg tasks, virtual chans, callback attachment
//this is now declared & implemented in util.h and util.cpp
//static int32 GetTerminalNameWithDevPrefix(TaskHandle taskHandle, const char terminalName[], char triggerName[]);
//int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
//int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

extern std::vector<TaskHandle> task_handle_vector;

int arte_setup_comm(); // set up the networking stuff

neural_daq new_neural_daq(boost::property_tree::ptree::value_type &v);

int save_setup_config(std::string &);
int save_session_config(std::string &);

void *wait_for_command(void *thread_data);

#endif
