#ifndef TRACKER_PROCESSOR_H_
#define TRACKER_PROCESSOR_H_

#include <mutex>
#include <vector>
#include "tracker_defs.h"
#include "tracker_pb.pb.h"
#include "../arte_pb.pb.h"

class point{
 public:
  double x;
  double y;
  double z;
  unsigned char l;
  double confidence;
};

typedef std::vector<point> point_set;
typedef std::vector<point_set> cams_points;
typedef std::vector<cams_points> groups_cams;

#define TRACKER_CAM_TO_WORLD 0
#define TRACKER_WORLD_TO_CAM 1
#define TRACKER_MAX_POINTS_TO_DETECT 10

class cam_points_info{
 public:
  point_set ps;
  int confidence;
};

class assignment_hypothesis{
 public:
  std::map <point, point> point_assignments;
  double assignment_score;
};


class TrackerProcessor {

 public:
  TrackerProcessor (TrackerOpt &o, 
                    multi_frame_collections_map& f,
                    std::mutex* m_p);

  ~TrackerProcessor();

  void update_opt(TrackerOpt *o);

  void process(ArtePb *pb);

 private:
  TrackerOpt opt;
  multi_frame_collections_map &frames_map;
  std::mutex *multi_frames_mutex_p;

  cam_points_info find_points_in_frame();
  
  point cam_and_world_coords(point p, const CameraOpt& c, int d);

  cam_points_info find_points_in_frame(ArteFrame *f, const CameraOpt &c);

  unsigned char neighbors_max(int x, int y, unsigned char *d, int stride);

  ArteFrame *scratch_frame;

  int ind2d(int x, int y, int stride);
  void inv_ind2d(int i, int stride, int *x, int *y);

};

#endif
