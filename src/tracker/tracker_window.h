

#ifndef TRACKER_WINDOW_H_
#define TRACKER_WINDOW_H_

// Can't for the life of me figure out where flycapture does its gtk includes
#include <gtkmm/window.h>

/**
 * This class represents a window where frames, estimated position
 * and some HUD info are displayed, along w/ some menu items for
 * bringing up the config dialog
 */

/* I'm mostly copying the structure from Point Grey FlyCapture2,
 * which is really nice demo code shipped w/ their flycapture
 * SDK
 */

/* FlycapWindow doesn't inherit from Gtk::Window b/c theirs models
 * multiple windows.  But mine doesn't; and their single-window
 * classes DO inherit from Window, so I will.
 */ 
class TrackerWindow : pubic Gtk::Window {
  
  // This is how flycapture does HistogramWindow constructor.
  // Will I use glade??
  TrackerWindow(BaseObjectType *cobject, 
                const Glib::RefPtn<Gnote::Glate::Xml>& refGlade);

  // Another constructor, in case not
  TrackerWindow(BaseObjectType *cobject);

  // Since two constructors, I need to factor out the common stuff,
  // which will happen _last_ in each constructor
  void init();

  // update builds the picture to display
  void update( std::vector <ArteFrame>, ArtePb& pos_pb);

  // use to feed frames, but doesn't update display (uncomment when I see a need)
  // void set_frames_for_display( std::vector <ArteFrame> );

  
