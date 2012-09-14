#include "tracker_window.h"

TrackerWindow::TrackerWindow(int argc, char *argv[]){

  init(argc, argv);

}

bool TrackerWindow::init(int argc, char *argv[]){

  m_pWindow = NULL;

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
   printf("test1\n");
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

}



void TrackerWindow::AttachSignals(){
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

}

void TrackerWindow::OnShowToolbar(){}

void TrackerWindow::OnConfigFile(){}
void TrackerWindow::OnConfigGUI(){}
void TrackerWindow::OnConfigSave(){}
void TrackerWindow::OnUseBackgroundSubtraction(){}
void TrackerWindow::OnAutoSnap(){}
void TrackerWindow::OnSnapAndUse(){}
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
  } else {
    if(recording){
      std::cout << "Can't stop acquiring during recording.\n";
      m_pAcquisitionButton->set_active(true);
    } else {
      std::cout << "Not Acquiring\n";
      acquiring = false;
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
