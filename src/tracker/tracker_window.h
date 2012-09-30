

#ifndef TRACKER_WINDOW_H_
#define TRACKER_WINDOW_H_

// Can't for the life of me figure out where flycapture does its gtk includes
#include <gtkmm.h>

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <libglademm/xml.h>
#include <vector>
#include "tracker_defs.h"
#include "../arte_pb.pb.h"

/**
 * This class represents a window where frames, estimated position
 * and some HUD info are displayed, along w/ some menu items for
 * bringing up the config dialog
 */

/* I'm mostly copying the structure from Point Grey FlyCapture2,
 * which is really nice demo code shipped w/ their flycapture
 * SDK
 */

class TrackerWindow {
  
 public:

  TrackerWindow(int argc, char *argv[],
                TRACKER_CALLBACK_FN2 run_callback, void *cb_data_r,
                TRACKER_CALLBACK_FN2 new_settings, void *cb_data_s);

  // update builds the picture to display
  void update( std::vector <ArteFrame>, ArtePb& pos_pb);

  // use to feed frames, but without updating display (uncomment when I see a need)
  // void set_frames_for_display( std::vector <ArteFrame> );

  bool Cleanup();


 private:

  /** Callback pointers and data **/
  TRACKER_CALLBACK_FN2 run_callback, new_settings;
  void *cb_data_r, *cb_data_s;

  /** Glade XML object (the libglade way)**/
  Glib::RefPtr<Gnome::Glade::Xml> m_refXml;

  /** GtkBuilder replacement (b/c my glade doesn't make libglade files)**/
  Glib::RefPtr<Gtk::Builder> builder;

  /** Window **/
  Gtk::Window *m_pWindow;

  /** Menu bar **/
  Gtk::MenuBar *m_pMenubar;

  /** Tool bar **/
  Gtk::Toolbar *m_pToolbar;

  /** Menu items **/
  Gtk::CheckMenuItem *m_pMenuShowToolbar;

  Gtk::MenuItem* m_pQuit;

  Gtk::MenuItem* m_pMenuConfigureFile;
  Gtk::MenuItem* m_pMenuConfigureGUI;
  Gtk::MenuItem* m_pMenuConfigureSave;
  
  Gtk::CheckMenuItem *m_pMenuHUD;
  Gtk::CheckMenuItem *m_pMenuThresholdView;

  Gtk::MenuItem* m_pMenuHelp;
  Gtk::MenuItem* m_pMenuAbout;

  /** Toolbar items **/
  Gtk::ToggleToolButton *m_pAcquisitionButton;
  Gtk::ToggleToolButton *m_pRecordingButton;

  Gtk::ToggleToolButton *m_pUseBackgroundSubtractionButton;
  Gtk::ToggleToolButton *m_pBackgroundAutoSnapButton;
  Gtk::ToolButton *m_pBackgroundSnapAndUseButton;

  Gtk::ToolButton *m_pSelectAllButton;
  Gtk::ToolButton *m_pSelectNoneButton;
  Gtk::ToolButton *m_pSelectNextButton;

  /** Status bar **/
  Gtk::Statusbar* m_pStatusBar;

  /** Last folder location that an image was saved to **/
  std::string m_saveImageLocation;

  guint status_ctx;

  bool acquiring;
  bool recording;

  int n_cameras;
  ArtePb tracker_opt;

  struct cam_opt {
    bool use_background_subtraction;
    bool auto_snap_background;
  };

  std::vector <cam_opt> cam_opts;

  /** Load widgets, attach signals, other init, return 0 for success **/
  bool init(int argc, char *argv[]);
  void GetWidgets();
  void AttachSignals();

  /** Flycapture defines separate callbacks for Menu and Toolbar
   *  versions of everything.  Is this really necessary?  I'm
   *  going to try without
   */
  bool OnDestroy( GdkEventAny *event );
  void OnAcquisition();

  void OnRecording();

  void OnShowToolbar();
  void OnQuit();
  void OnConfigFile();
  void OnConfigGUI();
  void OnConfigSave();
  void OnUseBackgroundSubtraction();
  void OnAutoSnap();
  void OnSnapAndUse();
  void OnHUD();
  void OnThreshView();
  void OnSelectAll();
  void OnSelectNone();
  void OnSelectNext();
  void OnHelp();
  void OnAbout();

  void UpdateStasusBar();

};



#endif
