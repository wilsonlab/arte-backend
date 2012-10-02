#ifndef FRAME_DRAWING_AREA_H_
#define FRAME_DRAWING_AREA_H_

#include <opencv2/core/core.hpp>
#include <FlyCapture2.h>
#include <mutex>
#include <gtkmm/drawingarea.h>
#include <gtkglmm.h>
#include "tracker_pb.pb.h"
#include "tracker_defs.h"

class FrameDrawingArea : public Gtk::GL::DrawingArea {

 public:
  
  FrameDrawingArea(TrackerOpt &opt, multi_frame_collections_map *the_frames);

  ~FrameDrawingArea();

  //  void set_images(multi_frame_collections_map *the_frames);
  void update_display();

  void set_running(bool running);

  void set_overlay(bool overlay);

  void get_mouse_coordinates(int *x_p, int *y_p);

  void get_preferred_size(int *width, int *height);

 protected:
  bool on_expose_event(GdkEventExpose *event);

 private:

  void  CommonInit();
  void InitializeImageTexture();
  void InitializeOpenGL();

  std::mutex *multi_frames_mutex_p;
  bool overlay;
  multi_frame_collections_map *the_frames;
  TrackerOpt my_opt;

  int height, width; // of the viewing window
  float scale_down_to_fraction;

  double SnapValue(double);

  static const unsigned int sk_maxNumTextures = 16;
  GLuint m_imageTextures[ sk_maxNumTextures ];

  bool BeginGL();
  void EndGL();
  void DestroyImageTexture();

  Glib::Mutex m_imageMutex;

};


#endif
