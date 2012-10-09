#ifndef FRAME_DRAWING_AREA_H_
#define FRAME_DRAWING_AREA_H_

#include <opencv2/core/core.hpp>
#include <FlyCapture2.h>
#include <mutex>
#include <gtkmm/drawingarea.h>
#include <gtkglmm.h>
#include <gtkmm.h>
#include "tracker_pb.pb.h"
#include "tracker_defs.h"
#include <X11/Xlib.h>

class FrameDrawingArea : public Gtk::GL::DrawingArea {

 public:

  FrameDrawingArea(multi_frame_collections_map *m);
  FrameDrawingArea(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);

  ~FrameDrawingArea();

  bool CommonInit(TrackerOpt &opt, multi_frame_collections_map *the_frames);

  //  void set_images(multi_frame_collections_map *the_frames);
  void update_display();

  void set_running(bool running);

  void set_overlay(bool overlay);

  void get_mouse_coordinates(int *x_p, int *y_p);

  void get_preferred_size(int *width, int *height);

  multi_frame_collections_map *frames_map;
  ArteFrame *reference_frame, *reference_frame2;
  char *signed_imageData, *unsigned_imageData;

  bool on_expose_event(GdkEventExpose *event);
  void on_realize();

  bool ready;

 private:

  bool fresh_frame;

  ArteFrame *selection;
  bool all_selected;
  std::string map_selection;

  void set_selection(ArteFrame *new_sel, bool all);
  void set_map_selection(std::string &new_sel);

  void assembleView();
  void my_blit_to_view(char *source, 
                       int sourceWidth, 
                       int sourceHeight,
                       int source_pix_bytes,
                       int dest_x, int dest_y,
                       int scale_down);
  
  void InitializeImageTexture();
  void InitializeOpenGL();

  std::mutex *multi_frames_mutex_p;
  bool overlay;
  multi_frame_collections_map *the_frames;
  TrackerOpt my_opt;

  int height, width; // of the viewing window
  double scale_down_to_fraction;

  double SnapValue(double);

  static const unsigned int sk_maxNumTextures = 16;
  GLuint m_imageTextures[ sk_maxNumTextures ];

  

  bool BeginGL();
  void EndGL();
  void DestroyImageTexture();

  bool on_configure_event(GdkEventConfigure* event);
  bool on_motion_notify_event(GdkEventMotion* event);
  bool on_button_release_event(GdkEventButton* event);


  bool m_isCurrentImage;
  double m_scaleX, m_scaleY;
  double m_shiftX, m_shiftY;

  unsigned int imageWidth, imageHeight;

};


#endif
