#include <opencv2/core/core.hpp>
#include "tracker_processor.h"

TrackerProcessor::TrackerProcessor(TrackerOpt &o,
                                   multi_frame_collections_map &f,
                                   std::mutex* m_p):
  opt(o),
  frames_map(f),
  multi_frames_mutex_p(m_p)
{
  scratch_frame = cvCreateImage( cvGetSize(frames_map["raw"][0][0]), IPL_DEPTH_8U, 1 );

}

void TrackerProcessor::process(ArtePb* pb){

  unsigned char* raw, back, back_sub;

  std::lock_guard <std::mutex> lk(*multi_frames_mutex_p);

  // loop over all the groups
  for( int g = 0; g < opt.group_size(); g++){

    // Initialize anything for this group?

    for( int c = 0; c < opt.group(g).cam_size(); c++){


      // loop over all cameras


      // get pointers to data of this camera
      char *raw       = frames_map["raw"][g][c]->imageData;
      char *bkgnd     = frames_map["background"][g][c]->imageData;
      char *bkgnd_sub = frames_map["background_subtracted"][g][c]->imageData;

      // std::cout << "bkgnd:fg[100-110]:";;
      // for(int i = 100; i <= 110; i++){
      //   std::cout << (int)  bkgnd[i] << ":" << (int) raw[i] << " ";
      // }
      // std::cout << std::endl;
      // std::cout << "'a' is " << (int) 'a' << "CHAR_MIN is" << (int) CHAR_MIN << std::endl;

      // Do background subtraction if it's requested.  Otherwise, just copy
      // the raw stuff into 'background_subtracted'
      if(! opt.group(g).cam(c).background_subtract() ){
        for(int p = 0; p < FRAME_WIDTH * FRAME_HEIGHT; p++){
          if((unsigned char) *raw >= ((unsigned char) *bkgnd + (unsigned char) 10)) {
            //*bkgnd_sub = (*raw) - (*bkgnd);
            *bkgnd_sub = *raw;
          } else {
            //*bkgnd_sub = (*bkgnd) - (*raw);
            *bkgnd_sub = 0;
          }
          bkgnd_sub++;
          raw++;
          bkgnd++;
        }
      } else {
        for(int p = 0; p < FRAME_WIDTH * FRAME_HEIGHT; p++){
          *bkgnd_sub =  p; // make a nonsense frame in 'background_sub;
          bkgnd_sub++;
        }
      }

      // What are then next steps for this camera?

    } // camera loop

    // Done with all cameras in this group.  Now do group-specific stuff


  } // group loop

  // done with all groups.  Do aggregate stuff

}

void TrackerProcessor::update_opt(TrackerOpt *o){
  opt = *o;

}

cam_points_info TrackerProcessor::find_points_in_frame(ArteFrame *f, const CameraOpt &c){

  cam_points_info cpi;

  // simplest way: brute force

  cvSmooth( f, scratch_frame, CV_GAUSSIAN, c.led_blur1(), c.led_blur1());

  int x,y;
  int step = 1;
  x = y = 1;
  unsigned char cum_max = 0;
  unsigned char* d = (unsigned char*) f->imageData;

  while (cpi.ps.size() < TRACKER_MAX_POINTS_TO_DETECT && y < (FRAME_HEIGHT - 1)){
    for(x = 1; x < FRAME_WIDTH; x += step){
      if (d[ind2d(x,y,FRAME_WIDTH)] >= neighbors_max(x,y,d, FRAME_WIDTH)){
        point p;
        p.x = x;
        p.y = y;
        cpi.ps.push_back(p);
      }
    }
    y =+ step;
  }
}

unsigned char TrackerProcessor::neighbors_max(int x, int y, unsigned char *d, int stride){
  unsigned char m = 0;
  m = d[ind2d(x-1, y-1,stride)] > m ? d[ind2d(x-1,y-1,stride)] : m;
  m = d[ind2d(x  , y-1,stride)] > m ? d[ind2d(x,  y-1,stride)] : m;
  m = d[ind2d(x+1, y-1,stride)] > m ? d[ind2d(x+1,y-1,stride)] : m;
  m = d[ind2d(x-1, y  ,stride)] > m ? d[ind2d(x-1,y  ,stride)] : m;
  m = d[ind2d(x+1, y  ,stride)] > m ? d[ind2d(x+1,y  ,stride)] : m;
  m = d[ind2d(x-1, y+1,stride)] > m ? d[ind2d(x-1,y+1,stride)] : m;
  m = d[ind2d(x  , y+1,stride)] > m ? d[ind2d(x  ,y+1,stride)] : m;
  m = d[ind2d(x+1, y+1,stride)] > m ? d[ind2d(x+1,y+1,stride)] : m;
  return m;
}

int TrackerProcessor::ind2d(int x, int y, int stride){
  return y*stride + x;
}

void TrackerProcessor::inv_ind2d(int i, int stride, int *x, int *y){
  *x = i % stride;
  *y = i / stride;
}


point TrackerProcessor::cam_and_world_coords(point p, const CameraOpt& c, int d){

  point ret_point;

  if (c.has_ortho_conv()){

    if( d == TRACKER_CAM_TO_WORLD){
      ret_point.x =
        p.x * (c.ortho_conv().x_max() - c.ortho_conv().x_min())  // scale
        + c.ortho_conv().x_max(); // shift
      ret_point.y =
        p.y * (c.ortho_conv().y_max() - c.ortho_conv().y_min())
        + c.ortho_conv().y_min();
    }

    else if (d == TRACKER_WORLD_TO_CAM){
      ret_point.x =
        (p.x - c.ortho_conv().x_min())
        / (c.ortho_conv().x_max() - c.ortho_conv().x_min());
      ret_point.y =
        (p.y - c.ortho_conv().y_min())
        / (c.ortho_conv().y_max() - c.ortho_conv().y_min());
    }

  }

  else if (c.has_affine_2d()){
    std::cerr << "2D affine transformation not implemented yet!\n";
    return point();
  }

  else if (c.has_proj_mat()){
    std::cerr << "3D projection not implemented yet!\n";
    return point ();
  }
}
