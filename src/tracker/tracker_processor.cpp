#include <opencv2/core/core.hpp>
#include "tracker_processor.h"

#define KERNEL_RANGE_COEFF 3

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

    for( int c = 0; c < opt.group(g).cam_size(); c++){      // loop over all cameras

      // get pointers to data of this camera
      char *raw       = frames_map["raw"][g][c]->imageData;
      char *bkgnd     = frames_map["background"][g][c]->imageData;
      char *bkgnd_sub = frames_map["background_subtracted"][g][c]->imageData;

      // Do background subtraction if it's requested.
      if(opt.group(g).cam(c).background_subtract() ){
        for(int p = 0; p < FRAME_WIDTH * FRAME_HEIGHT; p++){
          if((unsigned char) *raw >= ((unsigned char) *bkgnd + (unsigned char) 10)) {
            //*bkgnd_sub = (*raw) - (*bkgnd);  // true 'subtraction' of background
            *bkgnd_sub = *raw;  // <- instead, just do foreground-saving, background-removal
            //*bkgnd_sub = 255;
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
    assignment_hypothesis h = process_group( g );

  } // group loop

  // done with all groups.  Do aggregate stuff

}

assignment_hypothesis TrackerProcessor::process_group(int group_ind){

  int c_i;
  std::string f = "raw";
  for(c_i = 0; c_i < opt.group(group_ind).cam_size(); c_i++){
    if(opt.group(group_ind).cam(c_i).background_subtract())
      f = "background_subtracted";

    memset (frames_map["test"][group_ind][c_i]->imageData,
            (unsigned char) 0,
            FRAME_HEIGHT * FRAME_WIDTH);

    cam_points_info p = find_points_in_frame(frames_map[f][group_ind][c_i],
                                             opt.group(group_ind).cam(c_i),
                                             frames_map["test"][group_ind][c_i]);



    if(false){
      if(false){
        cvSmooth(frames_map[f][group_ind][c_i], frames_map["test"][group_ind][c_i],
                 CV_GAUSSIAN,
                 opt.group(group_ind).cam(c_i).led_blur1(),
                 opt.group(group_ind).cam(c_i).led_blur1()
                 //                 ,opt.group(group_ind).cam(c_i).led_blur1()
                 );
        //                 3,3);
      }
      if(false){
        memcpy (frames_map["test"][group_ind][c_i]->imageData,
                frames_map[f][group_ind][c_i]->imageData,
                FRAME_WIDTH * FRAME_HEIGHT );
      }
      if(false){
        for(int p = 1000; p < 3000; p++){
          frames_map["test"][group_ind][c_i]->imageData[p] = p;
        }
      }

    }

    //    if(p.ps.size() > 0)
    //   std::cout << "Got " << p.ps.size() << " points./n";
  }


  assignment_hypothesis h;
  return h;
}

void TrackerProcessor::recursive_flatten( unsigned char *s,
                                          unsigned char *r,
                                          int x, int y,
                                          unsigned char thresh,
                                          int *blob_sum_x, int *blob_sum_y, int *blob_px_count,
                                          unsigned char c){


  // stopping conditions
  if(x < 0 || x >= FRAME_WIDTH)
    return;
  if(y < 0 || y >= FRAME_HEIGHT)
    return;
  if (s[ind2d(x,y,FRAME_WIDTH)] < thresh ){
    return;
  }

  // for(int p = 0; p < (FRAME_WIDTH * FRAME_HEIGHT); p++)
  //   std::cout << (int) s[p];
  // std::cout << std::endl;

  // std::cout << "s[ind] = " << (int) s[ind2d(x,y,FRAME_WIDTH)] << "   thresh: " << (int)thresh << std::endl;

  // recursive case

  //  std::cout << "About to act. x: " << x << "  y: " << y << "  d[i]: "  << s[ind2d(x,y,FRAME_WIDTH)] << std::endl;


  s[ind2d(x,y,FRAME_WIDTH)] = 0;  // a small value
  r[ind2d(x,y,FRAME_WIDTH)] = c; // a viewable small value

  (*blob_sum_x) += x;
  (*blob_sum_y) += y;
  (*blob_px_count) += 1;

  recursive_flatten( s,r, x-1,  y  , thresh, blob_sum_x, blob_sum_y, blob_px_count, c );
  recursive_flatten( s,r, x+1,  y  , thresh, blob_sum_x, blob_sum_y, blob_px_count, c );
  recursive_flatten( s,r, x  ,  y-1, thresh, blob_sum_x, blob_sum_y, blob_px_count, c );
  recursive_flatten( s,r, x  ,  y+1, thresh, blob_sum_x, blob_sum_y, blob_px_count, c );


  // int new_x = x;
  // int new_y = y;
  // // trace left
  // while ((new_x >= 0) &&  (s[ ind2d(new_x, y, FRAME_WIDTH) ] >= thresh)){
  //   s[ind2d(new_x,y,FRAME_WIDTH)] = 0;
  //   r[ind2d(new_x,y,FRAME_WIDTH)] = 10;
  //   (*blob_sum_x) += x;
  //   (*blob_sum_y) += y;
  //   (*blob_px_count)++;
  //   new_x--;
  // }
  // // trace right
  // new_x = x+1;
  // while ((new_x < FRAME_WIDTH) &&  (s[ind2d(new_x, y, FRAME_WIDTH)]) >= thresh){
  //   s[ind2d(new_x,y,FRAME_WIDTH)] = 0;
  //   r[ind2d(new_x,y,FRAME_WIDTH)] = 10;
  //   (*blob_sum_x) += x;
  //   (*blob_sum_y) += y;
  //   (*blob_px_count)++;
  //   new_x++;
  // }
  // // trace up
  // while (new_y >= 0 &&
  //        s[ind2d(x, new_y, FRAME_WIDTH)] >= thresh){
  //   s[ind2d(x,new_y,FRAME_WIDTH)] = 0;
  //   r[ind2d(x,new_y,FRAME_WIDTH)] = 10;
  //   (*blob_sum_x) += x;
  //   (*blob_sum_y) += y;
  //   (*blob_px_count)++;
  //   new_y--;
  // }
  // // trace down
  // new_y = y+1;
  // while (new_y < FRAME_HEIGHT &&
  //        s[ind2d(x, new_y, FRAME_WIDTH)] >= thresh){
  //   s[ind2d(x,new_y,FRAME_WIDTH)] = 0;
  //   r[ind2d(x,new_y,FRAME_WIDTH)] = 10;
  //   (*blob_sum_x) += x;
  //   (*blob_sum_y) += y;
  //   (*blob_px_count)++;
  //   new_y++;
  // }



  // //  std::cout << "About to recurse up-left. x: " << x << "  y : " << y << std::endl;
  // recursive_flatten( s,r, x-1, y-1, thresh, blob_sum_x, blob_sum_y, blob_px_count );
  // //  std::cout << "About to recurse up-right. x: " << x << "  y : " << y << std::endl;
  // recursive_flatten( s,r, x+1, y-1, thresh, blob_sum_x, blob_sum_y, blob_px_count );
  // //  std::cout << "About to recurse down-left. x: " << x << "   y: " << y << std::endl;
  // recursive_flatten( s,r, x-1, y+1, thresh, blob_sum_x, blob_sum_y, blob_px_count );
  // //  std::cout << "About to recurse down-right. x:" << x << "  y: " << y << std::endl;
  // recursive_flatten( s,r, x+1, y+1, thresh, blob_sum_x, blob_sum_y, blob_px_count );

}


void TrackerProcessor::update_opt(TrackerOpt *o){
  opt = *o;

}

// void TrackerProcessor::build_kernels(){



// }

cam_points_info TrackerProcessor::find_points_in_frame(ArteFrame *f, const CameraOpt &c,
                                                       ArteFrame *result){

  cam_points_info cpi;

  // // simplest way: brute force
  // cvSmooth( f, scratch_frame, CV_GAUSSIAN, c.led_blur1(), c.led_blur1());
  // unsigned char t = (unsigned char) c.led_threshold();
  // t = 100;
  // point * p;
  // int x,y;
  // int step = 1;
  // x = y = 1;
  // unsigned char cum_max = 0;
  // unsigned char* d = (unsigned char*) f->imageData;
  // while (cpi.ps.size() < TRACKER_MAX_POINTS_TO_DETECT && y < (FRAME_HEIGHT - 1)){
  //   for(x = 1; x < FRAME_WIDTH; x += step){
  //     if (
  //         d[ind2d(x,y,FRAME_WIDTH)] > t &&
  //         d[ind2d(x,y,FRAME_WIDTH)] >= neighbors_max(x,y,d, FRAME_WIDTH)){
  //       p = new point;
  //       p->x = x;
  //       p->y = y;
  //       cpi.ps.push_back(*p);
  //       delete p;
  //     }
  //   }
  //   y += step;
  // }


  // a different way: scan whole image for threshold-crossing px's,
  // and then blob-detect each threshold-crosser, return list of blob centroids
  unsigned char *d = (unsigned char*) scratch_frame->imageData;
  int x, y, i;
  unsigned char thresh = (unsigned char) c.led_threshold();
  unsigned char *blob = (unsigned char*) result->imageData;
  point p;

  // for(int px = 0; px < FRAME_WIDTH * FRAME_HEIGHT; px++)
  //   std::cout << (int) d[px] << " ";
  // std::cout << std::endl;

  cvSmooth( f, scratch_frame, CV_GAUSSIAN, c.led_blur1(), c.led_blur1());

  for(x = 0; x < FRAME_WIDTH; x++){
    for(y = 0; y < FRAME_HEIGHT; y++){
      i = ind2d(x,y,FRAME_WIDTH);
      if( d[i] >= thresh ){
        // found threshold crosser
        int blob_x, blob_y, blob_c;
        blob_x = blob_y = blob_c = 0;
        //        std::cout << "About to rec_flat\n";
        recursive_flatten( d, blob, x, y, thresh, // let blobs spread with lower threshold
                           &blob_x, &blob_y, &blob_c, (unsigned char) 10*cpi.ps.size() % 200);
        //        std::cout << "Done with rec_flat\n";

        p.x = blob_x/blob_c;
        p.y = blob_y/blob_c;
        cpi.ps.push_back(p);
      }

    }
  }

  for(int b = 0; b < cpi.ps.size(); b++){
    //  std::cout << "b[" << b << "]: (" << cpi.ps[b].x << ", " << cpi.ps[b].y << ")" << std::endl;
  
    blob[ ind2d( cpi.ps[b].x, cpi.ps[b].y, FRAME_WIDTH ) ] = 255;
  }

  return cpi;

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
