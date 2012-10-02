/* TrackerDataSource is a collection of collections of camera frames.
   In a separate thread, asks all cameras for a frame, puts
   them into a buffer, and then waits 33ms and repeats.
   Each frame period, also call the callback specified by
   whoever owns TrackerDataSource */

#ifndef TRACKER_DATA_SOURCE_H_
#define TRACKER_DATA_SOURCE_H_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
//#include <cv.h>

#include "FlyCapture2.h"
#include "tracker_pb.pb.h"
#include "tracker_defs.h"

enum InputType { FLYCAP_IN, OPENCV_IN }; 
const InputType input_type = FLYCAP_IN;

enum bool_or_unset {BOU_UNSET, BOU_TRUE, BOU_FALSE};

class TrackerDataSource {

 public:
  /** 
   * TrackerDataSource constructor
   * 
   * @param opt           The protobuf tracker_opt for configuration
   * @param callback_p    The callback that will be called when a new
   *                      batch of frames arrives.
   * @param callback_data Additional data to be passed with the callback
   *                      Usually 'this' of the owner.
   */   
  TrackerDataSource( TrackerOpt &opt, 
                     TRACKER_CALLBACK_FN callback_p,
                     void *callback_data,
                     frame_collections *frames,
                     bool *main_running);

  ~TrackerDataSource();
  
  //  int run_cameras();  // this is now static, s_run_cameras
  static int s_run_cameras(TrackerDataSource *the_source);

 private:
  int init_cameras( );
  int init_files( );

  bool_or_unset file_sources;

  void print_camera_info( FlyCapture2::CameraInfo* pCamInfo );
  void print_flycapture_build_info();

  TrackerOpt opt;
  TRACKER_CALLBACK_FN callback_p;
  void *callback_data;

  bool *main_running;

  std::map <FlyCapture2::Camera*, ArteFrame*> physical_cameras_p;
  std::map <CvCapture*,           ArteFrame*> simulated_cameras_p;

  // Another list of the cameras, in format needed for
  // FlyCapture2::Camera::StartSyncCapture
  FlyCapture2::Camera **ppCameras;
 

  // pointer to frames vector vector owned by tracker.cpp
  frame_collections *frames;

  static void ptgr_err(FlyCapture2::Error);
  void list_serial_numbers();

  static void flycapture_to_opencv( FlyCapture2::Image*, IplImage* );

  // Display just for testing purposes
  bool draw;
  int which_cam;
  
  
};



#endif
