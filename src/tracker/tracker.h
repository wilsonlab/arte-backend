#ifndef TRACKER_H_
#define TRACKER_H_

#include "tracker_gui.h"
#include "tracker_data_source.h"
#include "tracker_processor.h"
#include "../glom.h"
#include "../netcom.h"
#include "../arte_pb.pb.h"
#include "../arte_command.pb.h"

/* Tracker class: Main class for rat tracker.
   
   Has pointers to 

   TrackerDataSource: which tracks
   all cameras, grabs images, collects images by
   camera grouping (in the case that there may
   be more than one camera monitoring the same track,
   for stereo reconstruction or pano-stitching, etc)
   Also handles saving of video data to .avi files,
   and reading from files if specified in the config.

   TrackerGui: Show results of tracking.  Toggle different
   parts of HUD (eg: show video?  Show LED's?  Show pos/dir?)

   TrackerProcessor: which processes images when they
   are made available by the TrackerDataSource.

*/

class Tracker {

 public:
  Tracker();
  ~Tracker();

 private:
  /* TrackerDataSource *data_source; */
  /* TrackerGui *gui; */
  /* TrackerProcessor *processor; */

  NetCom *pos_netcom;
  oGlom *output_file;

  TrackerOpt *tracker_opt;
  ArtePb     *pos_pb;

};


#endif
