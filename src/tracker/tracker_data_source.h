/* TrackerDataSource is a collection of collections of camera frames.
   In a separate thread, asks all cameras for a frame, puts
   them into a buffer, and then waits 33ms and repeats.
   Each frame period, also call the callback specified by
   whoever owns TrackerDataSource */

#ifndef TRACKER_DATA_SOURCE_H_
#define TRACKER_DATA_SOURCE_H_

#include <opencv.hpp>
#include "FlyCapture2.h"
#include "tracker_pb.pb.h"

enum InputType { FLYCAP_IN, OPENCV_IN }; 
const InputType input_type = FLYCAP_IN;

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
                     void *callback_data);
  

 private:
  int init_cameras( TrackerOpt &opt );
  int init_files( TrackerOpt &opt );

  void print_camera_info( FlyCapture2::CameraInfo* pCamInfo );
  void print_flycapture_build_info();

  TRACKER_CALLBACK_FN callback_p;
  void *callback_data;

};



#endif
