/* Tracker class: Main class for rat tracker.
   
   Has pointers to 

   TrackerDataSource: which tracks
   all cameras, grabs images, collects images by
   camera grouping (in the case that there may
   be more than one camera monitoring the same track,
   for stereo reconstruction or pano-stitching, etc)
   Also handles saving of video data to .avi files,
   and reading from files if specified in the config.

   TrackerGui: Show results of tracking.  Toggle different
   parts of HUD (eg: show video?  Show LED's?  Show pos/dir?)

   TrackerProcessor: which processes images when they
   are made available by the TrackerDataSource.

*/

#ifndef TRACKER_H_
#define TRACKER_H_

#include <thread>
#include "tracker_window.h"
#include "tracker_data_source.h"
#include "tracker_processor.h"
#include "tracker_pb.pb.h"
#include "../glom.h"
#include "../netcom.h"
#include "../arte_pb.pb.h"
#include "../arte_command.pb.h"


class Tracker {

 public:
  Tracker(int argc, char *argv[]);
  ~Tracker();

  static void handle_frames( void* cb_data );
  void m_handle_frames();
  frame_collections frames;

  bool main_running;

  static int n_run_calls;


 private:
  TrackerDataSource *tracker_data_source;
  TrackerWindow *gui;
  /* TrackerProcessor *processor; */

  std::thread *cam_thread;

  NetCom *pos_netcom;
  oGlom *output_file;

  TrackerOpt *tracker_opt;
  ArtePb     *pos_pb;

  // Read tracker_opt file, setup physical cameras
  // and the frame buffer into which the camera frames
  // will be written by tracker_data_source
  void init(int argc, char *argv[]);
  void start_gui(int argc, char *argv[], Tracker* the_tracker);
  
  void run(void* data);

  static void run_callback(void *cb_data, void *is_running);
  static void new_settings(void *cb_data, void *new_tracker_opt);


};

int Tracker::n_run_calls = 0;

#endif
