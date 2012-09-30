#include "tracker.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <thread>

Tracker::Tracker(int argc, char *argv[]){
  init(argc, argv);
  n_run_calls = 0;
  //tracker_data_source = new TrackerDataSource( *tracker_opt );

  std::thread gui_thread( &Tracker::start_gui, this, argc, argv, this );
  tracker_data_source = new TrackerDataSource(*tracker_opt,
                                              &handle_frames,
                                              (void*)this,
                                              &frames,
                                              &main_running);

  std::cout << "About to wait for gui" << std::endl;
  gui_thread.join();

}


Tracker::~Tracker(){
  std::cout << "Tracker: bye\n";
}

void Tracker::start_gui(int argc, char *argv[], Tracker* the_tracker_p){
  gui = new TrackerWindow(argc,argv,
                          &(the_tracker_p->run_callback),the_tracker_p,
                          &(the_tracker_p->new_settings),the_tracker_p);
}

void Tracker::handle_frames( void* cb_data ){
  ((Tracker*)cb_data)->m_handle_frames();
}

void Tracker::run_callback(void *cb_data, void *is_running){
  n_run_calls++;
  Tracker *t = (Tracker*) cb_data;
  t->main_running = *(bool*)is_running;
  std::cout << "run callback in MAIN. main_running is: " << t->main_running << std::endl;
  if((t->main_running) == true){
    std::cout << "About to start the thread\n"; fflush(stdout);
    if(t->cam_thread == NULL){
      t->cam_thread = new std::thread( &(TrackerDataSource::s_run_cameras), t->tracker_data_source );
    } else {
      std::cout << "Found non-null thread\n";
    }

    
    //    t->tracker_data_source->run_cameras();
    //    cam_thread.join();

  } else {
    if(t->cam_thread) {
      std::cout << "Waiting to join cam_thread\n"; fflush(stdout);

      t->cam_thread->join();
      std::cout << "Joined cam_thread\n"; fflush(stdout);
      delete (t->cam_thread);
    } else{
      std::cout << "No thread.  Nothing to do.\n"; fflush(stdout);
    }

    // No need to do anything - tracker_data_source can sense main_running
  } // end
  n_run_calls--;
  std::cout << "End of run_callback.  N-calls is now " << n_run_calls << std::endl;
}

void Tracker::new_settings(void *cb_data, void *new_tracker_opt){
  std::cout << "new_settings callback in MAIN\n";
}

void Tracker::m_handle_frames(){}

void Tracker::init(int argc, char *argv[]){
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  main_running = false;
  cam_thread = NULL;
  n_run_calls = 0;

  tracker_opt = new TrackerOpt ();

  std::string conf_path = std::string(getenv("HOME")) + "/.arte/tracker_default.conf";

  struct stat filestatus;
  if ((stat ( conf_path.c_str(), &filestatus )) == 0){ // file exists

    int file_desc = open(conf_path.c_str(), O_RDONLY);

    bool success = false;
    if(filestatus.st_size){
      google::protobuf::io::FileInputStream conf_stream(file_desc);
      success = google::protobuf::TextFormat::Parse( &conf_stream, tracker_opt );
    }
    printf("Filename: %s   Filesize: %d\n", conf_path.c_str(), filestatus.st_size);
    if(!success){
      std::cerr <<
        std::cerr << "Tracker Error: config file " << conf_path << " is invalid."
                  << "\n Please put a valid config file in ~/.arte/tracker_default.conf"
                  << " or run tracker with a path to a good config file.\n";
    }
  } else {  // file does not exist
    std::cerr << "No file found at " << conf_path
              << "\n Please put a valid config file in ~/.arte/tracker_default.conf"
              << " or run tracker with a path to a good config file.\n";
  }

  // Set up frames: the frame buffer for multiple camera groups
  // There are I camera groups, with J image pointers per group
  // (J is different from group to group).  For now, make those
  // pointers point to NULL
  for(int i = 0; i < tracker_opt->group_size(); i++){
    frames.push_back(frame_p_collection ());
    for(int j = 0; j < tracker_opt->group(i).cam_size(); j++){
      IplImage *this_image = cvCreateImage( cvSize(FRAME_WIDTH, FRAME_HEIGHT),
                                            IPL_DEPTH_8U, 1);
      frames[i].push_back( (ArteFrame*) NULL);
    }
  }

}


int main (int argc, char *argv[]){

  Tracker tracker(argc, argv);
  return EXIT_SUCCESS;

}
