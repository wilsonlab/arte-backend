#include "tracker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <google/protobuf/text_format.h>

Tracker::Tracker(){

  std::string conf_path = getenv("HOME") + "/.arte/tracker_pb.conf";
  google::protobuf::io::FileInputStream conf_stream(conf_path.c_str());
  google::protobuf::TextFormat::Parse( &conf_stream, tracker_opt );


}

