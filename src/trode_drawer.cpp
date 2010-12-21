#include "trode_drawer.h"

// change of plans: trode object will not know about trode_drawer object
// instead, trodes will keep a buffer of their recent spikes.
// trode_drawer will periodically (60 per sec?, in response to a gtk timeout loop?)
// ask for that buffer
// through Trode::get_spikes().  It will draw
// them, then clear them?  Or draw them and clear those that have been
// saved to disk?  Maybe instead it can set a 'drawn' flag.
// The trode can also keep 'saved', 'transmitted' flags for each spike,
// and clear all those that have been drawn, saved, trans, whatever else.

// a trode_drawer object then might inherit from Gtk::Window?  Gtk::DrawingArea?
// (in latter case, there would be a parent TrodeWindow inherriting from Gtk::Window) 
// (if these can be made opengl-able?).  Instantiated during 
// program init.  It will have an array of trode object pointers.
// It will also hold the drawing preferences (zoom, color, accumulate traces, etc)

TrodeDrawer::TrodeDrawer(){
}

TrodeDrawer::~TrodeDrawer(){
}

