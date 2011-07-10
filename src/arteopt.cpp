#include "arteopt.h"
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <assert.h>
#include "global_defs.h"
#include <iostream>
#include "util.h"
#include "timer.h"
#include "filtered_buffer.h"

FILE *main_file;

bool acquiring = false;

Timer arte_timer;
bool timer_is_toy = true;

std::string setup_config_filename;
std::string session_config_filename;
std::map<uint16_t, Trode> trode_map;
std::map<uint16_t, Lfp_bank> lfp_bank_map;

// preparing for the transition from maps to arrays
neural_daq          *neural_daq_array;
Filtered_buffer     *filtered_buffer_array;
Trode               *trode_array;
Lfp_bank            *lfp_bank_array;
NetCom a_netcom;
std::vector<NetCom>  netcom_vector;  // netcom is vector b/c I don't know that it has a fixed size

//NetCom          *netcom_array;

//Trode * trode_array = new Trode [ MAX_TRODES ];
//Lfp_bank * lfp_bank_array = new Lfp_bank [ MAX_LFP_BANKS ];
//Filtered_buffer * filtered_buffer_array = 
//  new Filtered_buffer [MAX_FILTERED_BUFFERS];

int n_neural_daqs;
int n_filtered_buffers;
int n_trodes;
int n_lfp_banks;
int n_netcoms;

// this can be left as a map.  Its contents (just options) 
// are copied in to the filtered_buffers, so we never
// come back and make reference to this map (no overhead probs) 
std::map<std::string, Filt> filt_map;


boost::property_tree::ptree setup_pt;
boost::property_tree::ptree session_pt;
boost::property_tree::ptree timer_pt;

std::vector<TaskHandle> task_handle_vector;

NetCom command_netcom;
NetComDat command_netcom_dat;

void arte_init(int argc, char *argv[], const std::string &setup_fn, const std::string &session_fn){

  timestamp = 0;

  if(!setup_fn.empty())
    setup_config_filename = setup_fn.data();
  else
    setup_config_filename = default_setup_config_filename.data();  
  if(!session_fn.empty())
    session_config_filename = session_fn;    
  else
    session_config_filename = default_session_config_filename.data();

  try{
    read_xml(setup_config_filename,   setup_pt,   boost::property_tree::xml_parser::trim_whitespace); // check where this flag actually lives
    read_xml(session_config_filename, session_pt, boost::property_tree::xml_parser::trim_whitespace); // can/should put 2 possible fails in one try block?
  }
  catch(...){  // find out where the xml_parse_error lives, & how to handle it
    std::cout << "XML read error was thrown - from arteopt.cpp" << std::endl;
  }

  arte_setup_init(argc, argv); // Use the property_tree to set global vars
  arte_session_init(argc, argv); // Use property_tree to set up trode list, trode/eeg view vars
  
//arte_network_init(argc, argv); // look at trode_array and lfp_bank_array, make netcom for each trode or lfp_bank, and 2 for arte itself
  //arte_init_timer();  // in timer.h
  //arte_start_clock(); // in timer.h 
  
  // Initialize the timer.  This will block until user hits return
  arte_timer.init2( setup_pt.get_child("options.setup.timer") );
  acquiring = true;
}


// Initialize the 'backplane' objects, such as the neural_daqs 
//and the pre-defined filters
void arte_setup_init(int argc, char *argv[]){

  n_neural_daqs = n_filtered_buffers = n_trodes = n_lfp_banks = n_netcoms = 0;
  
  neural_daq_init(setup_pt);

  BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		setup_pt.get_child("options.setup.filter_list")){
    Filt this_filt;
    boost::property_tree::ptree filt_pt;
    filt_pt = v.second;
    this_filt.init(filt_pt);
    filt_map.insert( std::pair<std::string, Filt>(this_filt.filt_name, this_filt) );
  }

  // initialize command port listener
  std::string command_port_str;
  char port_char[100];
  std::string command_host_str;
  char host_char[100];
  assign_property<std::string>("options.setup.command_port", &command_port_str, setup_pt, setup_pt, 1);
  assign_property<std::string>("options.setup.command_host", &command_host_str, setup_pt, setup_pt, 1);
  strcpy( port_char, command_port_str.c_str());
  strcpy( host_char, command_host_str.c_str());

  if( !strcmp("none",command_port_str.c_str()) == 0 ){
    int command_port_num;
    sscanf(command_port_str.c_str(), "%d", &command_port_num);
    command_netcom_dat = command_netcom.initUdpRx( host_char, command_port_num );
    int rc;
    long t;
    pthread_t command_thread;
    rc = pthread_create(&command_thread, NULL, wait_for_command,(void *)t);
    if(rc){
      printf("ERROR; return code from pthread_create() in arteopt command thread init is %d\n", rc);
      exit(1);
    }
  }

  std::cout << "Finished arte_setup_init." << std::endl;
}


// Initialize more session-specific features, such as the 
// mapping of daq channels into tetrode groups, and 
// of daq channels into lfp_banks (collections of eegs-channels
// all coming from a signle daq card and sharing filter settings
void arte_session_init(int argc, char *argv[]){

  std::cout << "Starting session init." << std::endl;

  // open the main data file
  char filename[MAX_NAME_STRING_LEN];
  std::string tmp_filename;
  assign_property<std::string>("options.session.main_filename",&tmp_filename, session_pt, session_pt, 1);
  strcpy( filename, tmp_filename.c_str());
  if( (strcmp(filename, "none")) != 0 ){
    main_file = try_fopen( filename, "wb" );
  } else{
    main_file = NULL;
  }

  // count how many trodes
  BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		session_pt.get_child("options.session.trodes")){
    n_trodes += 1;
    n_filtered_buffers += 1;
  }

  // count how many lfp_banks
  BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		session_pt.get_child("options.session.lfp_banks")){
    n_lfp_banks += 1;
    n_filtered_buffers += 1;
  }

  // Allocate the memory
  filtered_buffer_array = new Filtered_buffer [n_filtered_buffers];
  trode_array           = new Trode           [n_trodes];
  lfp_bank_array        = new Lfp_bank        [n_lfp_banks];

  // make itterators point to first element of each array
  Filtered_buffer * fb_curs = filtered_buffer_array;
  Trode           * trode_turs = trode_array;
  Lfp_bank        * lfp_bank_curs = lfp_bank_array;
  

  // *********** Start Initializing Trode Objects ************* //
  Trode trode_default(); // store the default settings here
  boost::property_tree::ptree default_trode_pt;
  boost::property_tree::ptree this_trode_pt;

  default_trode_pt = session_pt.get_child("options.session.trode_default");


  BOOST_FOREACH(boost::property_tree::ptree::value_type &v, 
		session_pt.get_child("options.session.trodes")){
    Trode this_trode;
    this_trode_pt = v.second;

    // ****** Old Trode initializer, for trode_map *******
    //this_trode.init(this_trode_pt, default_trode_pt, neural_daq_map, filt_map);
    //this_trode.print_options();
    //trode_map.insert( std::pair<uint16_t, Trode> ( this_trode.name, this_trode ));

    // new initializer of trodes in array
    std::istringstream iss (this_trode_pt.data()); // string corresponding to name
    int this_ind;
    iss >> this_ind;
    (trode_array+this_ind)->name = this_ind;
    (trode_array+this_ind)->init2(this_trode_pt,default_trode_pt, fb_curs);

    fb_curs++; // increment the cursor
  }

  Lfp_bank this_lfp_bank;
  BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		session_pt.get_child("options.session.lfp_banks")){
    boost::property_tree::ptree lfp_bank_pt;
    lfp_bank_pt = v.second;
    this_lfp_bank.init(lfp_bank_pt, neural_daq_map, filt_map);
    lfp_bank_map.insert( std::pair< uint16_t, Lfp_bank > ( this_lfp_bank.lfp_bank_name, this_lfp_bank) );
    
    std::istringstream iss (lfp_bank_pt.data());
    int this_ind;
    iss >> this_ind;
    (lfp_bank_array+this_ind)->init2(lfp_bank_pt, fb_curs);
    fb_curs++;
  }

  std::cout << "Done session init." << std::endl;

  // Start the acquisition
  neural_daq_start_all();

}

void *wait_for_command(void *thread_data){
  
//   int millisec = 100; // time for sleep
//   struct timespec req = {0};
//   req.tv_sec = 0;
//   req.tv_nsec = millisec * 1000000L;
//   for(int i = 0; i < 100; i++){
//     printf("GOT INTO WAIT_FOR_COMMAND.\n");
//     nanosleep(&req, (struct timespec*)NULL);
//   }

  command_t the_command;
  while(true){
    NetCom::rxCommand( command_netcom_dat, &the_command );
    printf("Got command:%s\n", the_command.command_str);
  }
  
  pthread_exit(NULL);
}
