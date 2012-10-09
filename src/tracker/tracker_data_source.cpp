#include "tracker_data_source.h"
#include <sys/time.h>
#include <thread>


TrackerDataSource::TrackerDataSource( TrackerOpt &opt,
                                      TRACKER_CALLBACK_FN callback_p,
                                      void *callback_data,
                                      frame_collections *frames,
                                      bool *main_running) :
  opt(opt),
  callback_p(callback_p),
  callback_data(callback_data),
  frames(frames),
  main_running(main_running)
{

  // input_type is meant to be a compile-time switch between flycapture sdk
  // and opencv for grabbing camera frames.
  if(input_type == FLYCAP_IN){

    // The expected situation.  Put all init code here?

  }
  else if(input_type == OPENCV_IN){
    std::cerr << "OPENCV_IN input mode not yet implemented." << std::endl;
  }
  else{
    std::cerr << "Unrecognized video input mode, set in tracker_data_source.h to "
              << "%d when in should be %d or %d."
              << input_type << FLYCAP_IN << OPENCV_IN << std::endl;
  }


  //  list_serial_numbers();

  init_cameras();


  // Drawing just to test capture
  draw = false;
  if(draw){
    which_cam = 2;
    cvNamedWindow( "Test drawing", CV_WINDOW_AUTOSIZE);
  }

}

TrackerDataSource::~TrackerDataSource(){

  if(draw){
    cvDestroyWindow( "Test drawing" );
  }

}


int TrackerDataSource::s_run_cameras(TrackerDataSource *s){

  std::cout << "HI from run_cams.  file_sources: " << s->file_sources << std::endl;

  // If we are using cameras, not files, then
  if(s->file_sources == BOU_FALSE){


    FlyCapture2::Error error;
    std::map<FlyCapture2::Camera*, ArteFrame*>::iterator it;
    std::cout << "About to start Capture\n";
    ptgr_err( error =
              FlyCapture2::Camera::StartSyncCapture( s->physical_cameras_p.size(),
                                                     (const FlyCapture2::Camera**)s->ppCameras) );
    if( error != FlyCapture2::PGRERROR_OK){
      std::cerr << "Error starting synced camera capture!/n";
    }

    // Grab and process frames, checking every time whether main says we're running
    it = s->physical_cameras_p.begin();
    FlyCapture2::Image image;
    std::cout << " *(s->main_running): " << *(s->main_running) << std::endl;
    while(*(s->main_running)){

      //std::lock_guard<std::mutex> lk(m_imageMutex);

      for(it = s->physical_cameras_p.begin(); it != s->physical_cameras_p.end(); it++){


        //        std::cout << "About to get frame\n";
        // Get the frame
        ptgr_err( error = it->first->RetrieveBuffer(&image) );
        if( error != FlyCapture2::PGRERROR_OK ){
          std::cerr << "Error retrieving frame from cam at address "
                    << it->first << std::endl;
        }
        //        std::cout << "Done getting frame\n";

        //        std::cout << "About to convert\n";
        // Copy frame to frame collection structure
        s->flycapture_to_opencv( &image, it->second );
        //        std::cout << "Done convert\n";

        // Draw the frame, maybe
        //        printf("it: %p  begin(): %p\n", it, s->physical_cameras_p.begin());
        //        if( s->draw && (it == s->physical_cameras_p.begin() ) ){
        if(s->draw){
          cvShowImage("Test drawing", it->second);
          //        std::thread t ( cvShowImage, "Test drawing", it->second);
          //        t.join();
        }

      } // end loop over cameras

      //      std::cout << "About to callback\n";

      // Issue the callback that was set by data_source's owner
      (*(s->callback_p))(s->callback_data);

      //      std::cout << "Done callback\n";

      // std::cout << "From run_cameras, main_running is now: "
      //           << *(s->main_running) << std::endl;

    } // end of main_running loop

    for(it = s->physical_cameras_p.begin(); it != s->physical_cameras_p.end(); it++){
      std::cout << "About to Stop()\n";
      it->first->StopCapture();
      std::cout << "Finished Stop()\n";
    }


  } // end if camera sources

  if(s->file_sources == BOU_TRUE){
    std::cout << "File reading not yet implemented, but tried to run that way.\n";
  } // end if file sources

  std::cout << "End of run_cameras\n";

}

void TrackerDataSource::flycapture_to_opencv( FlyCapture2::Image* f, IplImage* t){
  //printf("Converting, la la la ... f:%p  t:%p\n",f,t);
  int f_wd = f->GetCols();
  int f_ht = f->GetRows();
  float f_bytes_per_px = f->GetBitsPerPixel() / 8.0f;
  int t_wd = t->width;
  int t_ht = t->height;
  float t_bytes_per_px = (t->depth == IPL_DEPTH_8U) || (t->depth == IPL_DEPTH_8S)
    ? 1.0f : 2.0f;

  //  printf("f_wd: %d  f_ht: %d,  f_byte: %f,  t_wd: %d  t_ht: %d  t_byte: %f\n",
  //         f_wd, f_ht, f_bytes_per_px, t_wd, t_ht, t_bytes_per_px);
  
  if ( (t_wd == f_wd) &&
       (t_ht == f_ht) &&
       ((abs(t_bytes_per_px - f_bytes_per_px)) < 0.1) ){
    int rnd_bits = (abs(t_bytes_per_px - 8.0) < 0.1) ? 1 : 2;
    rnd_bits = 1;
    
    if(t->depth == IPL_DEPTH_8U)
      memcpy( t->imageData, f->GetData(), t_wd*t_ht*rnd_bits);
    
    
    if(t->depth == IPL_DEPTH_8S){ // convert from unsigned to signed
      for(int p = 0; p < FRAME_WIDTH * FRAME_HEIGHT; p++){
        //        t->imageData[p] = (f->GetData()[p] % (CHAR_MAX)) - (CHAR_MAX/2);
        //        t->imageData[p] = reinterpret_cast<signed char&> (f->GetData()[p]);
        t->imageData[p] = reinterpret_cast<char&> (f->GetData()[p]);
        printf("Wrong format?\n");
      }
    }
    
    int a = CHAR_MIN;
    //    printf("data[10] is: %c\n", t->imageData[10]);
  }
  
}

// Fill out physical_cameras_p vector
int TrackerDataSource::init_cameras(){

  // we don't yet know whether we're reading from camera or file
  file_sources = BOU_UNSET;

  FlyCapture2::BusManager busMgr;
  FlyCapture2::Error      error;
  unsigned int n_cams;
  ptgr_err( busMgr.GetNumOfCameras(&n_cams) );
  if(n_cams < 1){
    std::cerr << "No cameras found.\n";
    return -1;
  }

  int n_software_cams = 0;
  for(int g = 0; g < opt.group_size(); g++){
    for(int c = 0; c < opt.group(g).cam_size(); c++){

      n_software_cams++;

      bool_or_unset this_cam_from_file =
        opt.group(g).cam(c).has_input_file_name() ?  BOU_TRUE  : BOU_FALSE ;

      // Complain if some cameras want input files and others don't
      if( (this_cam_from_file != file_sources) && (file_sources != BOU_UNSET) ){
        std::cerr << "Configuration error: group " << g << " camera " << c
                  << " has input_file settings inconsistent with other cameras.";
      } else {
        file_sources = this_cam_from_file;
      }

      if(file_sources == BOU_FALSE){

        // Get guid
        FlyCapture2::PGRGuid guid;
        ptgr_err( error =
                  busMgr.GetCameraFromSerialNumber(opt.group(g).cam(c).serial_number(),
                                                   &guid) );
        if(error != FlyCapture2::PGRERROR_OK){
          std::cerr << "Error getting guid from serial number "
                    << opt.group(g).cam(c).serial_number()
                    << std::endl;
          list_serial_numbers();
        }

        // Connect
        FlyCapture2::Camera *this_camera_p = new FlyCapture2::Camera ();
        ptgr_err( error = this_camera_p->Connect( &guid ) );
        if(error != FlyCapture2::PGRERROR_OK){
          std::cerr << "Error connecting to camera with serial number "
                    << opt.group(g).cam(c).serial_number() << std::endl;
        }

        // Find videomode
        // I don't know the type of these macros, auto for now TODO: fix type
        auto the_mode = FlyCapture2::VIDEOMODE_640x480Y8;
        auto the_rate = FlyCapture2::FRAMERATE_30;
        if( FRAME_WIDTH == 640 && FRAME_HEIGHT == 480 ){
          the_mode = FlyCapture2::VIDEOMODE_640x480Y8;
        } else {
          std::cerr << "Unsupported image size\n";
        }
        if( FRAME_RATE == 30 ){
          the_rate = FlyCapture2::FRAMERATE_30;
        } else if (FRAME_RATE == 15) {
          the_rate = FlyCapture2::FRAMERATE_15;
        } else {
          std::cerr << "Unsupported frame rate\n";
        }


        // Setup camera
        ptgr_err( error = this_camera_p->
                  SetVideoModeAndFrameRate(the_mode, the_rate) );
        if( error != FlyCapture2::PGRERROR_OK ){
          std::cerr << "Error in setup for camera with serial number "
                    << opt.group(g).cam(c).serial_number() << std::endl;
        }

        // Register camera with tracker
        // Insert a pair into the camera listing map
        // camera_pointer -> image_pointer
        physical_cameras_p[this_camera_p] = (*frames)[g][c];

        // Initialize the capture
        // Build the camera list in the way that flycapture2 wants it: pointer array
        std::map<FlyCapture2::Camera*, ArteFrame*>::iterator it;
        int n_cams = physical_cameras_p.size();
        ppCameras = new FlyCapture2::Camera*[n_cams];
        it = physical_cameras_p.begin();
        for(int i = 0; i < n_cams; i++){
          ppCameras[i] = it->first;
          it++;
        }


      } // endif this camera really is camera soure

      if(file_sources == BOU_TRUE){

        // Create the opencv file capture
        CvCapture *this_file_capture =
          cvCreateFileCapture( opt.group(g).cam(c).input_file_name().c_str() );
        if(!this_file_capture){
          std::cerr << "TrackerDataSource error opening file "
                    << opt.group(g).cam(c).input_file_name().c_str() << std::endl;
        }

        // Register the file capture with tracker
        simulated_cameras_p[this_file_capture] = (*frames)[g][c];

      } // endif this camera has file source

    } // done with this camera
  } // done with this camera group


}

// Print point grey errors.
void TrackerDataSource::ptgr_err( FlyCapture2::Error error ){
  if(error != FlyCapture2::PGRERROR_OK){
    error.PrintErrorTrace();
    std::cout << "Press Enter to continue" << std::endl;
    getchar();
  }
}

// Copied from flycapture demo sources
void TrackerDataSource::print_camera_info( FlyCapture2::CameraInfo* pCamInfo ){
  using namespace FlyCapture2;
  printf(" \n Camera Info \n"
         "Serial: %u\n"
         "Model:  %s\n"
         "Vendor: %s\n"
         "Sensor: %s\n"
         "Resolution: %s\n"
         "Firmware version: %s\n"
         "Firmware built:   %s\n\n",
         pCamInfo->serialNumber, pCamInfo->modelName, pCamInfo->vendorName,
         pCamInfo->sensorInfo, pCamInfo->sensorResolution,
         pCamInfo->firmwareVersion, pCamInfo->firmwareBuildTime );
}

// Copied from flycapture demo sources
void TrackerDataSource::print_flycapture_build_info(){
  using namespace FlyCapture2;
  FC2Version fc2Version;
  Utilities::GetLibraryVersion( &fc2Version );
  char version[128];
  sprintf(version, "FlyCapture2 library version: %d.%d.%d.%d\n",
          fc2Version.major, fc2Version.minor, fc2Version.type, fc2Version.build );
  printf("%s", version);
  char timeStamp[512];
  sprintf( timeStamp, "Application build date: %s %s\n\n", __DATE__, __TIME__ );
  printf( "%s", timeStamp );
}

void TrackerDataSource::list_serial_numbers(){
  std::cout << "Serial numbers of cameras on the system: \n";
  FlyCapture2::BusManager busMgr;
  FlyCapture2::PGRGuid guid;
  unsigned int n_cams;
  FlyCapture2::CameraInfo camInfo;

  busMgr.GetNumOfCameras(&n_cams);

  FlyCapture2::Camera *this_camera = new FlyCapture2::Camera();

  for(int i = 0; i < n_cams; i++){

    busMgr.GetCameraFromIndex(i, &guid);

    this_camera->Connect( &guid );

    this_camera->GetCameraInfo( &camInfo );
    std::cout << "Camera " << i
              << " serial number: " << camInfo.serialNumber
              << std::endl;

    this_camera->Disconnect();
  }

  delete this_camera;

  std::cout << "Serial numbers requested in config file" << std::endl;
  for(int g = 0; g < opt.group_size(); g++){
    for(int c = 0; c < opt.group(g).cam_size(); c++){
      std::cout << "Group " << g << " camera " << c
                << " serial number: " << opt.group(g).cam(c).serial_number()
                << std::endl;
    }
  }


}
