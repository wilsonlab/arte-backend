

#ifndef TRACKER_DEFS_H_
#define TRACKER_DEFS_H_

#include <opencv.hpp>
#define OPENCV_FRAMES
typedef IplImage ArteFrame;

// Define TRACKER_CALLBACK_FN function pointer
// takes one argument returns nothing
//   (in the future, use signals instead?)
typedef void (*TRACKER_CALLBACK_FN)(void *);

const bool VERBOSE = true;

#endif
