#include "tracker_data_source.h"
#include <sys/time.h>

TrackerDataSource::TrackerDataSource( TrackerOpt &opt,
                                      TRACKER_CALLBACK_FN callback_p,
                                      void *callback_data) :
  callback_p(callback_p),
  callback_data(callback_data)
{
  
  // input_type is meant to be a compile-time switch between flycapture sdk
  // and opencv for grabbing camera frames.
  if(input_type == FLYCAP_IN){
    

  }
  else if(input_type == OPENCV_IN){
    std::cerr << "OPENCV_IN input mode not yet implemented." << std::endl;
  }
  else{
    std::cerr << "Unrecognized video input mode, set in tracker_data_source.h to " 
              << "%d when in should be %d or %d." 
              << input_type << FLYCAP_IN << OPENCV_IN << std::endl;
  }

  // This is test-acquisition code only.
  // using namespace cv;
  //  cvNamedWindow("win1", CV_WINDOW_AUTOSIZE);
  //  cvMoveWindow("win1", 100, 100);

  //  bool running = true;
  //  char key = 0;
  //  double n_frames = 0;

  //  timeval tim,tim_0;
  //  gettimeofday(&tim_0,NULL);

  //  CvCapture *capture = cvCaptureFromCAM(0);

  //  cvSetCaptureProperty( capture, CV_CAP_PROP_FPS, 30.0 );

  //  IplImage *img = 0;

  //  while(running){
  //    if(!cvGrabFrame(capture)){
  //      printf("No frame\n");
  //      exit(0);
  //    }
  //    img = cvRetrieveFrame(capture);
  //    cvShowImage("win1",img);
  //    n_frames += 1.0;
  //    gettimeofday(&tim,NULL);
  //    key = cvWaitKey(1);
  //    if ((tim.tv_sec - tim_0.tv_sec) > 1.0)
  //      printf("key: %d fps:%f\n",key, n_frames /(tim.tv_sec -tim_0.tv_sec));
  //    if(key == 27)
  //      running = false;
  //  }

  // Another version of simple test-acquisition code.
   //  VideoCapture cap(0); // open the default camera
   //  //    if(!cap.isOpened())  // check if we succeeded
   //          //  return -1;
   //  double n_frames = 0.0;
   // timeval tim,tim_0;
   // gettimeofday(&tim_0,NULL);


   //  Mat edges;
   //  namedWindow("edges",1);
   //  for(;;)
   //  {
   //      Mat frame;
   //              cap >> frame; // get a new frame from camera
   //      //cvtColor(frame, edges, CV_BGR2GRAY);
   //      GaussianBlur(frame, frame, Size(7,7), 1.5, 1.5);
   //      //Canny(frame, frame, 0, 30, 3);
   //   n_frames += 1.0;
   //   gettimeofday(&tim,NULL);

   //   if ((tim.tv_sec - tim_0.tv_sec) > 1.0)
   //     printf("fps:%f\n", n_frames /(tim.tv_sec -tim_0.tv_sec));
   //   //        imshow("edges", frame);
   //   cvShowImage("edges",frame);
   //      if(waitKey(30) >= 0) break;
   //  }
    
}


// Copied from flycapture demo sources
TrackerDataSource::print_camera_info( FlyCapture2::CameraInfo* pCamInfo ){
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
TrackerDataSource::print_flycapture_build_info(){
  using namespace FlyCapture2;
  FC2Version fc2Versior;
  Utilities::GetLibraryVersion( &fc2Versior );
  char version[128];
  sprintf(version, "FlyCapture2 library version: %d.%d.%d.%d\n",
          fc2Version.major fc2Version.minor, fc2Version.type, fc2Version.build );
  printf("%s", version);
  char timeStamp[512];
  sprintf( timeStamp, "Application build date: %s %s\n\n", __DATE__, __TIME__ );
  printf( "%s", timeStamp );
}
