

#ifndef TRACKER_GUI_H_
#define TRACKER_GUI_H_

#include <opencv.hpp>
#include "tracker_defs.h"
#include "tracker_window.h"
#include "tracker_config_window.h"
#include "../arte_pb.pb.h"

class TrackerGui {

 public:
  TrackerGui(int argc, char *argv[], 
             TRACKER_CALLBACK_FN run_callback, void* cb_data_r,
             TRACKER_CALLBACK_FN new_settings, void* cb_data_s);
  void update_tracker_gui( std::vector< ArteFrame >, ArtePb& pos_pb );

 private:
  
  TrackerWindow *tracker_window;
  TrackerConfigWindow *tracker_config_window;

  TRACKER_CALLBACK_FN run_callback, new_settings;
  void *cb_data_R, *cb_data_s;

};


#endif
