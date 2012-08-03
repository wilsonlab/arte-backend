// Convert the old boost property_tree representation of
// config files to the protobuf format

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <exception>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/foreach.hpp>
#include <google/protobuf/text_format.h>
#include "arte_pb.pb.h"
#include "glom.h"

using std::string;


class Converter{

public:
  Converter(int argc, char *argv[]){

    parse_args(argc, argv);
    open_files();
    if(doing_setup) translate_setup();
    if(doing_session) translate_session();

    if(verbose) print_args_map();

    // We want to use a single protobuf, not a glom,
    // because glom comes with the bit count on top, which will
    // be out of date if file is edited by hand.
    //my_glom -> pb_write(my_pb); 

    std::cout << "Test in main of has_arte_session: " << my_pb.has_arte_session() << std::endl;

    do_output();

  }

  bool verbose;
  std::string prog_name;


  boost::property_tree::ptree setup_pt;
  boost::property_tree::ptree session_pt;
  bool doing_setup, doing_session;
  oGlom *my_glom;
  std::ofstream output_file;

  ArtePb my_pb;
  ArteSessionOptPb *my_session_pb; // method scope b/c used between two functions: 
                                   // translate_session and find_source_trode
  ArteTrodeOptPb * default_trode_pb; // this will be used in find_source_trode, too
  ArteLfpOptPb * default_lfpbank_pb; // and this

  void do_output(){
    const char m = my_args["output_mode"][0];
    if (m == 'a') {
      std::cout << "HELLO\n" << std::endl;
      std::string tmp ("Test");
      std::cout << "Success: " << google::protobuf::TextFormat::PrintToString( my_pb, &tmp ) << std::endl;
      std::cout << tmp;
      output_file << tmp;
      std::cout << "Is-initialized:" << my_pb.has_arte_session() << std::endl;
    }

  }

  void translate_setup(){
    std::cout << "HI!\n";;
    ArteSetupOptPb *my_setup_pb = my_pb.mutable_arte_setup();
    int n_samps_per_buffer; // hold a copy of this value for later use in filter config
    // setup each neural daq
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		  setup_pt.get_child("options.setup.neural_daq_list")){
      auto this_daq = v.second;
      ArteNeuralDaqOptPb *this_daq_pb = my_setup_pb->add_daqs();
      this_daq_pb->set_id(  this_daq.get <uint32_t>(std::string ("id")));
      this_daq_pb->set_dev_name( this_daq.get<std::string>(std::string("dev_name")));
      this_daq_pb->set_in_filename(this_daq.get<std::string>(std::string("in_filename")));
      this_daq_pb->set_out_filename(this_daq.get<std::string>(std::string("raw_dump_filename")));
      this_daq_pb->set_buffer_time_samps(this_daq.get<uint32_t>(std::string("n_samps_per_buffer")));
      n_samps_per_buffer = this_daq_pb->buffer_time_samps(); // copy any daq's samp count for later
      int n_chans = this_daq.get<int>(std::string("n_chans"));
      this_daq_pb->set_buffer_n_chans(n_chans);
      for(int n = 0; n < n_chans; n++){
	ArteChanOptPb *this_chan = this_daq_pb->add_chans();
	this_chan->set_gain(1);
	this_chan->set_daq_chan(n);
      }
      this_daq_pb->set_is_master( this_daq_pb->id() == 0);
      this_daq_pb->set_is_multiplexing(false);
    }

    ArteCommandPortOptPb *command_port = my_setup_pb->mutable_command_port();
    command_port->set_command_host( setup_pt.get<std::string>(std::string("options.setup.command_host")));
    command_port->set_port( setup_pt.get<std::string>(std::string("options.setup.command_port")));
    command_port->set_secondary_port( setup_pt.get<std::string>(std::string("options.setup.secondary_command_port")));

    BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		  setup_pt.get_child("options.setup.hosts")){
      auto this_host_pt = v.second;
      ArteHostOptPb *this_host_pb = my_setup_pb->add_host_list();
      this_host_pb->set_host(v.first);
      this_host_pb->set_ip_addy(this_host_pt.get<std::string>(std::string("ip_addy")));
    }

    BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		  setup_pt.get_child("options.setup.filter_list")){
      ArteFilterOptPb *filt_pb = my_setup_pb->add_filters();
      auto this_filter_pt = v.second;
      filt_pb->set_filter_name( this_filter_pt.get_value<string>() );
      std::istringstream num (this_filter_pt.get<std::string>(std::string("num_coefs")));
      while(!num.eof()){
	double n;
	num >> n;
	filt_pb->add_numerators(n);
      }
      std::istringstream den (this_filter_pt.get<std::string>(std::string("denom_coefs")));
      while(!den.eof()){
	double n;
	den >> n;
	filt_pb->add_denominators(n);
      }
      std::istringstream mul (this_filter_pt.get<std::string>(std::string("input_gains")));
      while(!mul.eof()){
	double n;
	mul >> n;
	filt_pb->add_multiplier(n);
      }
      filt_pb->set_order(this_filter_pt.get<uint32_t>(std::string("order")));
      int n = this_filter_pt.get<uint32_t>("filtfilt_wait_n_buffers");
      // here is the later use of n_samps_per_buffer from neural daq setup
      filt_pb->set_filtfilt_invalid_samps( n * n_samps_per_buffer );
      filt_pb->set_regenerate_coefs( false );
    }

  }

  void a(){
    printf("Hi.\n");
  }

  void translate_session(){
    my_session_pb = my_pb.mutable_arte_session();
    my_session_pb->set_main_filename( session_pt.get<std::string>(string("options.session.main_filename")));
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		  session_pt.get_child("options.session.trodes")){
      ArteTrodeOptPb *trode_pb = my_session_pb->add_trodes();
      auto this_trode_pt = v.second;
      translate_trode(trode_pb, this_trode_pt);
    }
    default_trode_pb = my_session_pb->mutable_default_trode();
    translate_trode(default_trode_pb, session_pt.get_child("options.session.trode_default"));

    std::cout << "About to translate default\n";
    default_lfpbank_pb = my_session_pb->mutable_default_lfpbank();
    auto this_lfpbank_pt = session_pt.get_child("options.session.lfp_banks.lfp_bank");
    translate_lfp( default_lfpbank_pb, this_lfpbank_pt );
      //    default_lfpbank_pb->CopyFrom(my_session_pb->lfpbanks(0));
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		  session_pt.get_child("options.session.lfp_banks")){
      ArteLfpOptPb *lfp_pb = my_session_pb->add_lfpbanks();
      auto this_lfpbank_pt = v.second;
      std::cout << "About to translate lfp in loop.\n";
      translate_lfp(lfp_pb, this_lfpbank_pt);
    }

  }

  void translate_lfp( ArteLfpOptPb* lfp_pb, boost::property_tree::ptree &lfp_pt){
    using std::istringstream;
    using std::vector;
    try{
      lfp_pb->set_source_lfpbank(lfp_pt.get_value<int>());
      lfp_pb->set_lfp_opt_version(0);
    } catch (...) {}
    try {
      string filt_name = lfp_pt.get<string>("filt_name");
      lfp_pb->set_filter_name(filt_name);
    } catch (...) {}
    int daq_id = 100; // keep this in translate_lfp scope so that find_source_trode can use it later
    try {
      daq_id = lfp_pt.get<int>(string("daq_id"));
      lfp_pb->set_daq_id(daq_id);
    } catch (...){
      daq_id = default_lfpbank_pb->daq_id();
    }
    try {
      bool network = lfp_pt.get<bool>(string("network"));
      lfp_pb->set_network(network);
    } catch(...) {}
    try { 
      bool disk = lfp_pt.get<bool>(string("disk"));
      lfp_pb->set_disk(disk);
    } catch (...){}
    try {
      string ip = lfp_pt.get<string>(string("ip_addy"));
      lfp_pb->set_host_ip(ip);
    } catch (...) {}
    try {
      string port = lfp_pt.get<string>(string("port"));
      lfp_pb->set_port(port);
    } catch(...) {}
    try {
      int keep_nth = lfp_pt.get<int>(string("keep_nth_sample"));
      lfp_pb->set_keep_nth_sample(keep_nth);
    } catch(...) {}
    int min_chans = 1000;
    vector <double> gains;
    try {
      istringstream gains_s (lfp_pt.get<string>(string("gains")));
      while(!gains_s.eof()){
	double g;
	gains_s >> g;
	gains.push_back(g);
      }
      min_chans = std::min(int(min_chans), int(gains.size()));
    } catch (std::exception &e) {std::cout << e.what() << std::endl;} // yep, catching
    vector <int> daq_chans;
    try {
      istringstream chans_s (lfp_pt.get<string>(string("channels")));
      while(!chans_s.eof()){
	int c;
	chans_s >> c;
	daq_chans.push_back(c);
      }
      min_chans = std::min(int(min_chans), int(daq_chans.size()));
    } catch (...) {}
    
    for(int n = 0; n < min_chans; n++){
      ArteChanOptPb *this_chan = lfp_pb->add_chans();
      if( daq_chans.size() > 0){
	this_chan->set_daq_chan(daq_chans[n]);
	try {
	  int source_trode = find_source_trode(daq_id, daq_chans[n]);
	  this_chan->set_source_trode(source_trode);
	} catch (...) {}
      }
      if( gains.size() > 0)
	this_chan->set_gain(gains[n]);
    }

  }

  int find_source_trode(int daq_id, int chan){
    for(int t = 0; t < my_session_pb->trodes_size(); t++){
      ArteTrodeOptPb *this_trode = my_session_pb->mutable_trodes(t);
      int this_daq_id =
	this_trode->has_daq_id() ? this_trode->daq_id() : default_trode_pb->daq_id();
      if(this_daq_id == daq_id){
	if( this_trode->chans_size() > 0 && this_trode->mutable_chans(0)->has_daq_chan()){
	  for(int c = 0; c < this_trode->chans_size(); c++){
	    //printf("test this_trode daq chan: %d  requested  chan: %d\n",this_trode->mutable_chans(c)->daq_chan(), chan );
	    if ((this_trode->mutable_chans(c))->daq_chan() == chan){
	      return this_trode->source_trode();
	    }
	  }
	} else { 
	  for(int c = 0; c < default_trode_pb->chans_size(); c++){
	    if ((default_trode_pb->mutable_chans(c))->daq_chan() == chan)
	      return this_trode->source_trode();
	  }
	  
	}
	
      }
    }
    printf("Found no matching trode in find_source_trode for daq_id: %d  chan: %d\n",daq_id,chan);
    //      throw std::exception;
    return -1;
    
    
}

  void translate_trode(ArteTrodeOptPb* trode_pb, boost::property_tree::ptree &trode_pt){
    using std::istringstream;
    using std::vector;
    try {
      trode_pb->set_source_trode(trode_pt.get_value<int>());
      trode_pb->set_trode_opt_version(0);
    } catch (...) {a();} // no catches in test

    try {
      int daq_id = trode_pt.get<int>(string("daq_id"));
      trode_pb->set_daq_id(daq_id);
    } catch(...) {}
    try {
      string filt_name = trode_pt.get<string>(string("filt_name"));
      trode_pb->set_filter_name(filt_name);
    } catch (...) {}
    try {
      int samps_before_trig = trode_pt.get<int>(std::string("samps_before_trig"));
      trode_pb->set_samps_before_trig(samps_before_trig);
    } catch(...) {}
    try {
      int samps_after_trig = trode_pt.get<int>(std::string("samps_after_trig"));
      trode_pb->set_samps_after_trig(samps_after_trig);
    } catch(...) {}
    try {
      int refrac = trode_pt.get<int>(std::string("refractory_period_samps"));
      trode_pb->set_refractory_period_samps(refrac);
    } catch(...) {}
    try {
      string host = trode_pt.get<string>(string("host"));
      trode_pb->set_host_ip(host);
    } catch(...) {}
    try {
      string port = trode_pt.get<string>(string("port"));
      trode_pb->set_port(port);
    } catch(...) {}
    try {
      bool disk = trode_pt.get<bool>(string("disk"));
      trode_pb->set_disk(disk);
    } catch(...) {}
    try {
      bool net = trode_pt.get<bool>(string("network"));
      trode_pb->set_network(net);
    } catch(...) {}
    int lowest_chan_count = 1000;
    vector <double> gains;
    double tmp;
    try {
      istringstream gains_s (trode_pt.get<string>(string("gains")));
      while(!gains_s.eof()){
	gains_s >> tmp;
	gains.push_back(tmp);
      }
      lowest_chan_count = std::min((int)lowest_chan_count, (int)gains.size());
    } catch (...){}
    vector <int> daq_chans;
    int tmp2;
    try {
      istringstream chans_s (trode_pt.get<string>(string("channels")));
      while (!chans_s.eof()){
	chans_s >> tmp2;
	daq_chans.push_back(tmp2);
      }
      lowest_chan_count = std::min((int)lowest_chan_count, (int)daq_chans.size());
    } catch (...) {}
    vector <double> thresholds;
    try {
      istringstream thresh_s (trode_pt.get<string>(string("thresholds")));
      while (!thresh_s.eof()){
	thresh_s >> tmp;
	thresholds.push_back(tmp);
      }
      lowest_chan_count = std::min((int)lowest_chan_count, (int)thresholds.size());
    } catch (...) {}
      
    for(int n = 0; n < lowest_chan_count; n++){
      ArteChanOptPb *this_chan = trode_pb->add_chans();
      if( gains.size() > 0)
	this_chan->set_gain(gains[n]);
      if( daq_chans.size() > 0)
	this_chan->set_daq_chan(daq_chans[n]);
      if( thresholds.size() > 0)
	this_chan->set_threshold(thresholds[n]);
      this_chan->set_source_trode(trode_pt.get_value<int>());
    }
    



  }

  void print_help(){
    std::cout << std::endl << "Usage: " << prog_name 
	      << " [OPTION] ... [--setup-file SETUP_FILE] "
	      << "[--session-file SESSION_FILE] -o OUTPUT_FILE" << std::endl
	      << "Convert arte backend property_tree xml settings files into google "
	      << "protocol buffers, which are the new config format." 
	      << std::endl << std::endl
	      << " -a, --ascii      output to ascii protobuf format" << std::endl
	      << " -b, --binary     output to binary protubuf format (default)" 
	      << std::endl
	      << " -v, --verbose" << std::endl
	      << " -h, --help, --usage" << std::endl << std::endl;
  }

  void open_files(){
    try{
      if(my_args["session_input"].empty()){
	doing_session = false;
      }else {
	read_xml(my_args["session_input"], session_pt, 
		 boost::property_tree::xml_parser::trim_whitespace);
	doing_session = true;
      }
      if(my_args["setup_input"].empty()){
	doing_setup = false;
      }else{
	read_xml(my_args["setup_input"], setup_pt, 
		 boost::property_tree::xml_parser::trim_whitespace);
	doing_setup = true;
      }
    } 
    catch(std::exception &e) {
      std::cerr << "Error opening input file.  Exception: " << e.what();
    }
    try{
      const char m = my_args["output_mode"][0];
      
      if (m == 'a'){
	output_file.open(my_args["output_file"], std::ios::out);
      }
      if (m == 'b') {
	output_file.open(my_args["output_file"], std::ios::out | std::ios::binary );
      }
      
      //my_glom = new oGlom ( my_args["output_file"].c_str(), m);
    }
    catch(std::exception &e) {
      std::cerr << "Error opening output file. Exception: " << e.what();
    }
  }


  typedef std::pair<std::string, std::string> sp;
  typedef std::map <std::string, std::string> arg_map;
  // This function is defined later.  parse_args will build a string->string map

  arg_map my_args;

  // parse_args will build a string->string map for command-line arguments
  // ["prog_name", "convert_config_to_proto"]
  // ["setup_input",   "./conf/arte_default_setup.conf"]
  // ["session_input", "./conf/arte_default_session.conf"]
  // ["verbose", "yes"]
  // ["output_file", "./conf/backend_settings.conf"]
  // ["output_mode", "b"]
  void parse_args(int argc, char *argv[]) {

    prog_name = argv[0];

    // set defaults
    std::vector<bool> ok_flag (argc, false);
    my_args.insert (sp("output_mode","b"));
    my_args.insert (sp("verbose", "no"));
    my_args.insert (sp("setup_input", ""));
    my_args.insert (sp("session_input", ""));
    ok_flag[0] = true;
    try {
      for (int n = 1; n < argc; n++){
	if (strcmp(argv[n], "--session-file") == 0) {
	  if (n + 1 >= argc) { 
	    throw std::string("--session-file is last argument");
	  }
	  my_args.erase("session_input");
	  my_args.insert (sp("session_input", argv[n+1]));
	  ok_flag[n] = true;
	  ok_flag[n+1] = true;
	}
	if (strcmp(argv[n], "--setup-file") == 0){
	  if (n + 1 >= argc) { throw std::string("--setup-file is last argument");}
	  my_args.erase("setup_input");
	  my_args.insert (sp("setup_input", argv[n+1]));
	  ok_flag[n] = true;
	  ok_flag[n+1] = true;
	}
	if (strcmp(argv[n], "-o") == 0){
	  if (n + 1 >= argc) {throw std::string("-o is last argument");}
	  my_args.erase("output_file");
	  my_args.insert (sp("output_file", argv[n+1]));
	  ok_flag[n] = true;
	  ok_flag[n+1] = true;
	}
	if (strcmp(argv[n], "-a") == 0 || strcmp(argv[n], "--ascii") == 0){
	  my_args.erase("output_mode");
	  my_args.insert (sp("output_mode","a"));
	  ok_flag[n] = true;

	}
	if (strcmp(argv[n], "-b") == 0|| strcmp(argv[n], "--binary") == 0){
	  my_args.erase("output_mode");
	  my_args.insert (sp("output_mode","b"));
	  ok_flag[n] = true;
	}      
	if (strcmp(argv[n], "-v") == 0 || strcmp(argv[n], "--verbose") == 0){
	  my_args.erase("verbose");

	  my_args.insert(sp("verbose","yes"));
	  verbose = true;
	  ok_flag[n] = true;
	}
	if (strcmp(argv[n], "-h") == 0 || strcmp(argv[n], "--help") == 0
	    || strcmp(argv[n], "--usage") == 0){
	  print_help();
	  exit(0);
	}
      }
    }
    
    catch (std::string n){
      std::cout << "Error: Bad arguments list: " << n << std::endl;
      exit(1);
    }
    catch(std::exception &e){
      std::cout << "Caught unexpected exception.  what(): " << e.what() << std::endl;
    }

    bool any_bad = false;

    for(int n = 0; n < argc; n++){
      if (ok_flag[n] == false){
	std::cout << "Unrecognized argument " << n << ": " << argv[n] << std::endl;
	any_bad = true;
      }
    }
    if (any_bad) {
      std::cout << "Error: Bad arguments.  Try running with --usage flag." << std::endl;
      exit(0);
    }
    if (argc < 2) {
      print_help();
      print_args(argc, argv);
      std::cout << std::endl << "Error: Too few arguments." << std::endl << std::endl;
      exit (0);
    }

  } // end parse_args()

  void print_args_map(){
    std::cout << "my_args size: " << my_args.size() << std::endl;
    for(auto it = my_args.begin(); it != my_args.end(); it++)
      std::cout << it->first << " -> " << it->second << std::endl;
    std::cout << std::endl;
  }
  void print_args(int argc, char *argv[]){
    std::cout << std::endl << "Command line arguments: " << std::endl;
    for(int n = 0; n < argc; n++)
      std::cout << n << " " << argv[n] << std::endl;
  }

};

int main(int argc, char *argv[]){

  Converter my_converter(argc, argv);

  exit(0);

}
