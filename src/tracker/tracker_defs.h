

#ifndef TRACKER_DEFS_H_
#define TRACKER_DEFS_H_

#include <opencv.hpp>
#define OPENCV_FRAMES
typedef IplImage ArteFrame;

// Define TRACKER_CALLBACK_FN function pointer
// takes one argument returns nothing
//   (in the future, use signals instead?)
typedef void (*TRACKER_CALLBACK_FN)(void *);
typedef void (*TRACKER_CALLBACK_FN2)(void *, void*);
typedef void (*TRACKER_CALLBACK_FN3)(void *, void *, void *);

// Define image_collections
typedef std::vector <ArteFrame*> frame_p_collection;
typedef std::vector <frame_p_collection> frame_collections;

const bool VERBOSE = true;

const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
const int FRAME_RATE = 30;

#endif
