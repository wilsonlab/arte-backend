

#ifndef TRACKER_GUI_H_
#define TRACKER_GUI_H_

#include <opencv.hpp>
#include "tracker_defs.h"
#include "../arte_pb.pb.h"

class TrackerGui {

 public:
  void update_tracker_gui( std::vector< ArteFrame >, ArtePb& pos_pb );

 private:
  
  TrackerWindow *tracker_window;
  TrackerConfigWindow *tracker_config_window;

}
