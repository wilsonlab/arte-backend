/* TrackerDataSource is a collection of collections of camera frames.
   In a separate thread, asks all cameras for a frame, puts
   them into a buffer, and then waits 33ms and repeats.
   Each frame period, also call the callback specified by
   whoever owns TrackerDataSource */

#ifndef TRACKER_DATA_SOURCE_H_
#define TRACKER_DATA_SOURCE_H_

#include <opencv.hpp>
#include "tracker_pb.pb.h"

class TrackerDataSource {

 public:
  TrackerDataSource( TrackerOpt &opt );


};


#endif
