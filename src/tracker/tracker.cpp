#include "tracker.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

Tracker::Tracker(int argc, char *argv[]){
  init(argc, argv);
  //tracker_data_source = new TrackerDataSource( *tracker_opt );

  gui = new TrackerGui(argc, argv);

}


Tracker::~Tracker(){
  std::cout << "Tracker: bye\n";
}
 
void Tracker::init(int argc, char *argv[]){
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  tracker_opt = new TrackerOpt ();

  std::string conf_path = std::string(getenv("HOME")) + "/.arte/tracker_default.conf";
 
  struct stat filestatus;
  if ((stat ( conf_path.c_str(), &filestatus )) == 0){ // file exists

    int file_desc = open(conf_path.c_str(), O_RDONLY);
    
    bool success = false;
    if(filestatus.st_size){
      google::protobuf::io::FileInputStream conf_stream(file_desc);
      success = google::protobuf::TextFormat::Parse( &conf_stream, tracker_opt );
    }
    printf("Filename: %s   Filesize: %d\n", conf_path.c_str(), filestatus.st_size);
    if(!success){
      std::cerr << 
        std::cerr << "Tracker Error: config file " << conf_path << " is invalid."
                << "\n Please put a valid config file in ~/.arte/tracker_default.conf"
                << " or run tracker with a path to a good config file.\n";
    }
  } else {  // file does not exist
    std::cerr << "No file found at " << conf_path 
              << "\n Please put a valid config file in ~/.arte/tracker_default.conf"
              << " or run tracker with a path to a good config file.\n";
  }


}


int main (int argc, char *argv[]){

  Tracker tracker(argc, argv);
  return EXIT_SUCCESS;

}
