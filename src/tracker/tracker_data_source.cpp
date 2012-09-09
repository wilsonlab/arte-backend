#include "tracker_data_source.h"
#include <sys/time.h>

TrackerDataSource::TrackerDataSource( TrackerOpt &opt ){
  using namespace cv;
   cvNamedWindow("win1", CV_WINDOW_AUTOSIZE);
   cvMoveWindow("win1", 100, 100);

   bool running = true;
   char key = 0;
   double n_frames = 0;

   timeval tim,tim_0;
   gettimeofday(&tim_0,NULL);

   CvCapture *capture = cvCaptureFromCAM(0);

   cvSetCaptureProperty( capture, CV_CAP_PROP_FPS, 30.0 );

   IplImage *img = 0;

   while(running){
     if(!cvGrabFrame(capture)){
       printf("No frame\n");
       exit(0);
     }
     img = cvRetrieveFrame(capture);
     cvShowImage("win1",img);
     n_frames += 1.0;
     gettimeofday(&tim,NULL);
     key = cvWaitKey(1);
     if ((tim.tv_sec - tim_0.tv_sec) > 1.0)
       printf("key: %d fps:%f\n",key, n_frames /(tim.tv_sec -tim_0.tv_sec));
     if(key == 27)
       running = false;
   }

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


