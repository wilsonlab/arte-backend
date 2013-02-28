#include "tracker.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <thread>
#include "tracker_processor.h"

std::mutex m_imageMutex;

Tracker::Tracker(int argc, char *argv[]){
  init(argc, argv);
  n_run_calls = 0;

  // Start the data source
  tracker_data_source = new TrackerDataSource(*tracker_opt,
                                              &handle_frames,
                                              (void*)this,
                                              &frames_map["raw"],
                                              &main_running);

  // Start the processor
  tracker_processor = new TrackerProcessor( *tracker_opt,
                                            frames_map,
                                            multi_frames_mutex_p );

  // Start the gui
  std::thread gui_thread( &Tracker::start_gui, this, argc, argv, this );

  std::cout << "About to wait for gui to finish" << std::endl;
  gui_thread.join();

}


Tracker::~Tracker(){
  std::cout << "Tracker: bye\n";
}

void Tracker::start_gui(int argc, char *argv[], Tracker* the_tracker_p){
  TrackerWindow *tmp_window_addy;
  gui = new TrackerWindow(argc,argv,
                          &(the_tracker_p->run_callback),the_tracker_p,
                          &(the_tracker_p->new_settings),the_tracker_p,
                          *tracker_opt, &frames_map);
  std::cout << "Gui addy in tracker is: " << gui 
            << ". Now doing init." << std::endl;
  gui->init(argc, argv);
}

void Tracker::handle_frames( void* cb_data ){
  ((Tracker*)cb_data)->m_handle_frames();
}
void Tracker::m_handle_frames(){
  tracker_processor->process(&pos_pb);
  //  gui->refresh_frames();
  //  gui->update(pos_pb.mutable_arte_pos(), gui);
}

void Tracker::run_callback(void *cb_data, void *is_running){

  n_run_calls++;
  Tracker *t = (Tracker*) cb_data;
  t->main_running = *(bool*)is_running;

  if( (*(bool*)is_running) == true){  // transitioned into running mode
    std::cout << "About to start the thread\n"; fflush(stdout);
    if(t->cam_thread == NULL){
      t->cam_thread = new std::thread( &(TrackerDataSource::s_run_cameras), 
                                       t->tracker_data_source );
    } else {
      std::cout << "Found non-null thread when trying to start acquiring\n";
    }

  } else { // Transitioned to not-running
    if(t->cam_thread) {

      t->cam_thread->join();
      delete (t->cam_thread);
      t->cam_thread = NULL;

    } else{
      std::cout << "Tried to stop acq. But no acqisition thread.  Nothing to do.\n"; 
      fflush(stdout);
    }

  } // end
  n_run_calls--;
  std::cout << "End of run_callback.  N-calls is now " << n_run_calls << std::endl;
}

void Tracker::new_settings(void *cb_data, void *new_tracker_opt){
  std::cout << "new_settings callback in MAIN\n";
}


void Tracker::init(int argc, char *argv[]){
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  main_running = false;
  cam_thread = NULL;
  n_run_calls = 0;

  multi_frames_mutex_p = new std::mutex ();

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

  frame_collections frames, backgrounds, frames_background_subtracted, frames_hybrid, test;

  for(int i = 0; i < tracker_opt->group_size(); i++){

    frames.push_back(frame_p_collection ());
    backgrounds.push_back(frame_p_collection ());
    frames_background_subtracted.push_back(frame_p_collection ());
    frames_hybrid.push_back(frame_p_collection ());
    test.push_back(frame_p_collection ());

    for(int j = 0; j < tracker_opt->group(i).cam_size(); j++){

      IplImage *this_image_a = cvCreateImage( cvSize(FRAME_WIDTH, FRAME_HEIGHT),
                                              IPL_DEPTH_8U, 1);
      IplImage *this_image_bk= cvCreateImage( cvSize(FRAME_WIDTH, FRAME_HEIGHT),
                                              IPL_DEPTH_8U, 1);
      IplImage *this_image_b = cvCreateImage( cvSize(FRAME_WIDTH, FRAME_HEIGHT),
                                              IPL_DEPTH_8U, 1);
      IplImage *this_image_c = cvCreateImage( cvSize(FRAME_WIDTH, FRAME_HEIGHT),
                                              IPL_DEPTH_8U, 1);
      IplImage *this_image_t = cvCreateImage( cvSize(FRAME_WIDTH, FRAME_HEIGHT),
                                              IPL_DEPTH_8U, 1);

      for(int p = 0; p < FRAME_WIDTH*FRAME_HEIGHT; p++){

        this_image_a->imageData[p] = 0;
        this_image_bk->imageData[p] = 0;
        this_image_b->imageData[p] = 0;
        this_image_c->imageData[p] = 0;
        this_image_t->imageData[p] = 0;

      }

      frames[i].push_back( (ArteFrame*) this_image_a);
      backgrounds[i].push_back( (ArteFrame*) this_image_bk);
      frames_background_subtracted[i].push_back( (ArteFrame*) this_image_b );
      frames_hybrid[i].push_back( (ArteFrame*) this_image_c );
      test[i].push_back( (ArteFrame*) this_image_t );

    }
  }

  // A map will organize the different processing stages of each frame
  frames_map["raw"] = frames;
  frames_map["background"] = backgrounds;
  frames_map["background_subtracted"] = frames_background_subtracted;
  frames_map["hybrid"] = frames_hybrid;
  frames_map["test"] = test;
}


int main (int argc, char *argv[]){

  Tracker tracker(argc, argv);
  return EXIT_SUCCESS;

}
