// trode_drawer class
// a type of Gtk::DrawingArea with a list of trodes, a list of drawing-areas, and the mapping
// Keeps track of
// 
//  * a list of peaks (or other waveform features?) for all past spikes 
//    (possibly just bitmap renderings of projections, w/ a white dot added at each new spike)
// From config:
// * a list of the drawing areas (openGL or matlab) to draw to
// * for each trode, a list of which channels to render waveforms for (probably all)
// * for each trode, a list of which projections to draw
// * for each trode, other preferences (line color, overlay mode, etc.)

#include <iostream>
#include "opts.h"
#include "spike.h"
// include opengl stuff

class TrodeDrawer{

 public:
  TrodeDrawer();
  virtual ~TrodeDrawer();

  void remove_all_drawingareas();
  void attach_drawingarea();

  void new_spike(Spike); // Make a spike struct 


 private:
  n_chans;
  n_samps_per_chan;

  // add vars for display options

  // a list of channel waveform plots

  // somehow manage a list of projection plots, their x/y lims
  // an array of bitmaps (to collect spikes)

};
