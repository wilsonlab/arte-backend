#include "tracker_window.h"


//http://stackoverflow.com/questions/9299187/adding-an-opengl-window-into-a-gtk-builder

TrackerWindow::TrackerWindow(int argc, char *argv[],
                             TRACKER_CALLBACK_FN2 run_callback, void *cb_data_r,
                             TRACKER_CALLBACK_FN2 new_settings, void *cb_data_s,
                             TrackerOpt &o, multi_frame_collections_map* m) :
  run_callback(run_callback), cb_data_r(cb_data_r),
  new_settings(new_settings), cb_data_s(cb_data_s),
  frames_map(m), tracker_opt(o)
{
  //  my_addy_out = this;
  //  init(argc, argv);
  //run();

}

void TrackerWindow::update(ArtePosPb* p_pb, TrackerWindow* t){
  //  std::cout << "This trackerwindow is " << this << std::endl;
  //  std::cout << "About to queue_draw().  Addy is: " << m_pArea << std::endl; fflush(stdout);
  //  int a,b;
  //  m_pArea->set_can_focus(true);
  //  std::cout << "Finished set_can_focus()\n"; fflush(stdout);
  //  m_pArea->show_now();
  //  m_pArea->get_preferred_width(a, b);
  //m_pArea->show();
  //  std::cout << "Show went ok\n";
  std::cout << "UPDATE\n";
  m_pArea->on_expose_event((GdkEventExpose*) NULL);
}

bool TrackerWindow::init(int argc, char *argv[]){
  std::cout << "Init done\n";
  m_pWindow = NULL;

  m_pArea = NULL;

  m_pMenubar = NULL;
  m_pMenuShowToolbar = NULL;

  m_pQuit = NULL;
  m_pMenuConfigureFile = m_pMenuConfigureGUI = m_pMenuConfigureSave = NULL;
  m_pMenuHUD = m_pMenuThresholdView = NULL;
  m_pMenuHelp = m_pMenuAbout = NULL;

  m_pAcquisitionButton = m_pRecordingButton = NULL;
  m_pSelectAllButton = m_pSelectNoneButton = m_pSelectNextButton = NULL;
  m_pStatusBar = NULL;
  m_saveImageLocation.clear();

  acquiring = recording = false;

  Gtk::Main kit(argc, argv);

  const char *glade_main_window = "tracker_window.glade";

  try{
    builder = Gtk::Builder::create_from_file(glade_main_window);
  } catch(const Gnome::Glade::XmlError &ex) {
    std::cerr << "Error opening glade file " << glade_main_window << std::endl;
    return false;
  }

  builder->get_widget("window", m_pWindow);
  if( m_pWindow == NULL ){
    std::cerr << "Error parsing glade file.\n";
    return false;
  }

  GetWidgets();
  AttachSignals();
  std::cout << "Init done.\n";
  Gtk::Main::run(*m_pWindow);

}

bool TrackerWindow::on_timeout(){

  if(m_pArea->ready){

    m_pArea->on_expose_event((GdkEventExpose*) NULL);

  } else {
    printf("Not ready!\n");
  }
  return true;
}

void TrackerWindow::run(){
  std::cout << "m_pWindow is: " << m_pWindow << "  and 'this' is " << this << std::endl;
  //  Gtk::Main(0,0,false);
  Gtk::Main::run(*m_pWindow);
}

void TrackerWindow::GetWidgets(){


  builder->get_widget("statusbar", m_pStatusBar);
  guint c = m_pStatusBar->get_context_id("Sys");
  m_pStatusBar->push("Initializing.... GetWidgets()",status_ctx);

  builder->get_widget("acquisition",     m_pAcquisitionButton);
  builder->get_widget("recording",       m_pRecordingButton);
  builder->get_widget("select_all",      m_pSelectAllButton);
  builder->get_widget("select_none",     m_pSelectNoneButton);
  builder->get_widget("select_next",     m_pSelectNextButton);
  builder->get_widget("use_bkgnd_sub",   m_pUseBackgroundSubtractionButton);
  builder->get_widget("auto_bkgnd_snap", m_pBackgroundAutoSnapButton);
  builder->get_widget("bkgnd_snap_use",  m_pBackgroundSnapAndUseButton);

  builder->get_widget("quit_item",       m_pQuit);
  builder->get_widget("load_config_item",m_pMenuConfigureFile);
  builder->get_widget("save_config_item",m_pMenuConfigureSave);
  builder->get_widget("config_GUI_item", m_pMenuConfigureGUI);
  builder->get_widget("view_HUD_item",   m_pMenuHUD);
  builder->get_widget("bkgnd_view_item", m_pMenuThresholdView);
  builder->get_widget("help_item",       m_pMenuHelp);
  builder->get_widget("about_item",      m_pMenuAbout);

  Gtk::Box *vb = NULL;
  builder->get_widget("da_box", vb);

  //  m_pArea = new FrameDrawingArea((*frames_map)["raw"][0][0]->imageData);
  m_pArea = new FrameDrawingArea(frames_map);
  m_pArea->show();
  vb->add(*m_pArea);

}



void TrackerWindow::AttachSignals(){

  m_pArea->signal_expose_event().connect(sigc::mem_fun(*m_pArea,
                                                       &FrameDrawingArea::on_expose_event));

  m_pWindow->signal_expose_event().connect(sigc::mem_fun(*this,
                                                         &TrackerWindow::OnExpose));

  m_pStatusBar->push("Initializing... AttachSignals()",status_ctx);

  m_pWindow->signal_delete_event().connect(sigc::mem_fun( *this,
                                                          &TrackerWindow::OnDestroy));
  m_pQuit->signal_select().connect(sigc::mem_fun(*this, &TrackerWindow::OnQuit));
  m_pAcquisitionButton->signal_toggled().connect(sigc::mem_fun(*this,
                                                               &TrackerWindow::OnAcquisition));
  m_pRecordingButton->signal_clicked().connect(sigc::mem_fun(*this,
                                                             &TrackerWindow::OnRecording));
  m_pSelectAllButton->signal_clicked().connect(sigc::mem_fun(*this,
                                                             &TrackerWindow::OnSelectAll));
  m_pSelectNoneButton->signal_clicked().connect(sigc::mem_fun(*this,
                                                              &TrackerWindow::OnSelectNone));
  m_pSelectNextButton->signal_clicked().connect(sigc::mem_fun(*this,
                                                              &TrackerWindow::OnSelectNext));
  m_pUseBackgroundSubtractionButton->
    signal_clicked().connect(sigc::mem_fun(*this, &TrackerWindow::OnUseBackgroundSubtraction));
  m_pBackgroundAutoSnapButton->
    signal_clicked().connect(sigc::mem_fun(*this, &TrackerWindow::OnAutoSnap));
  m_pBackgroundSnapAndUseButton->
    signal_clicked().connect(sigc::mem_fun(*this, &TrackerWindow::OnSnapAndUse));

  m_pMenuConfigureFile->signal_select().connect(sigc::mem_fun(*this,
                                                              &TrackerWindow::OnConfigFile));
  m_pMenuConfigureSave->signal_select().connect(sigc::mem_fun(*this,
                                                              &TrackerWindow::OnConfigSave));
  m_pMenuConfigureGUI->signal_select().connect(sigc::mem_fun(*this,
                                                             &TrackerWindow::OnConfigGUI));
  m_pMenuHUD->signal_toggle().connect(sigc::mem_fun(*this,
                                                    &TrackerWindow::OnHUD));
  m_pMenuThresholdView->signal_toggle().connect(sigc::mem_fun(*this,
                                                              &TrackerWindow::OnThreshView));
  m_pMenuHelp->signal_select().connect(sigc::mem_fun(*this, &TrackerWindow::OnHelp));
  m_pMenuAbout->signal_select().connect(sigc::mem_fun(*this,&TrackerWindow::OnAbout));

  m_pArea->CommonInit(tracker_opt, frames_map);



  sigc::slot<bool> my_slot =
    sigc::mem_fun(*this, &TrackerWindow::on_timeout);
  sigc::connection conn = Glib::signal_timeout().connect(my_slot, 25);


}

bool TrackerWindow::OnExpose(GdkEventExpose *event){

  //  std::cout << "Expose event from window\n";

  // if(m_pArea == NULL){
  //   printf("NULL mPArea!!");
  // }
  // if(m_pArea->ready == true){
  //   printf("About to m_pArea->on_expose_event(event)\n");
  //   if(!m_pArea->on_expose_event(event)){
  //     std::cerr << "Tried and failed an on_expose_event\n";
  //   }
  // } else{
  //   printf("Not ready!\n");
  // }


}

void TrackerWindow::OnShowToolbar(){}

void TrackerWindow::OnConfigFile(){}
void TrackerWindow::OnConfigGUI(){}
void TrackerWindow::OnConfigSave(){}
void TrackerWindow::OnUseBackgroundSubtraction(){}
void TrackerWindow::OnAutoSnap(){}

void TrackerWindow::OnSnapAndUse(){

  std::cout << "SNAP AND USE\n";

  for(int g = 0; g < tracker_opt.group_size(); g++){
    for(int c = 0; c < tracker_opt.group(g).cam_size(); c++){
      memcpy( (*frames_map)["background"][g][c]->imageData,
              (*frames_map)["raw"][g][c]->imageData,
              FRAME_WIDTH * FRAME_HEIGHT );
    }
  }

}

void TrackerWindow::OnHUD(){}
void TrackerWindow::OnThreshView(){}
void TrackerWindow::OnSelectAll(){}
void TrackerWindow::OnSelectNone(){}
void TrackerWindow::OnSelectNext(){}
void TrackerWindow::OnHelp(){}
void TrackerWindow::OnAbout(){}

void TrackerWindow::OnAcquisition(){

  if(!acquiring){
    std::cout << "Acquiring.\n";
    acquiring = true;
    run_callback( cb_data_r, &acquiring );
  } else {
    if(recording){
      std::cout << "Can't stop acquiring during recording.\n";
      m_pAcquisitionButton->set_active(true);
    } else {
      std::cout << "Not Acquiring\n";
      acquiring = false;
      run_callback( cb_data_r, &acquiring );
    }
  }
}

void TrackerWindow::OnRecording(){
  if(!recording){
    std::cout << "Recording.\n";
    recording = true;
  }  else {
    std::cout << "Not Recording.\n";
    recording = false;
  }

}

bool TrackerWindow::OnDestroy(GdkEventAny *event){
  std::cout << "Destroy signal\n";
  return false;
}


void TrackerWindow::OnQuit(){
  std::cout << "Quit\n";
}
