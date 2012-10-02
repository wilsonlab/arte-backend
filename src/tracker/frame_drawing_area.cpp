#include "frame_drawing_area.h"
#include <GL/glu.h>

FrameDrawingArea::FrameDrawingArea(TrackerOpt &opt,
                                   multi_frame_collections_map *the_frames):
  my_opt(opt),
  the_frames(the_frames)
{
  CommonInit();
}

FrameDrawingArea::~FrameDrawingArea(){
  DestroyImageTexture();
}

bool FrameDrawingArea::on_expose_event(GdkEventExpose *event){

}

void FrameDrawingArea::InitializeImageTexture(){

  glGenTextures( sk_maxNumTextures, m_imageTextures );

  bool useClampToEdge = true;
  if( atof( (const char*)glGetString(GL_VERSION)) < 1.15 ){
    useClampToEdge = false;
  }

  for( unsigned int i = 0; i < sk_maxNumTextures; i++ ){
    glBindTexture( GL_TEXTURE_2D, m_imageTextures[i] );

    auto c = GL_CLAMP_TO_EDGE;
    if(!useClampToEdge)
      c = GL_CLAMP;
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  }

}



void FrameDrawingArea::update_display(){

}

void FrameDrawingArea::get_preferred_size(int *w, int *h){
  *w = width;
  *h = height;
}

// Mostly copied from FlyCapture2 ImageDrawingArea.cpp
// the FlyCap2 src examples
void FrameDrawingArea::CommonInit(){

  scale_down_to_fraction = 1.0 / my_opt.max_n_cams();

  Gtk::GL::init(0, NULL);

  // Display is: Main screen, with max_n_cams little screens underneath
  height = FRAME_HEIGHT + (int)((float)FRAME_HEIGHT * scale_down_to_fraction);
  width  = FRAME_WIDTH;

  Gdk::EventMask mask =
    Gdk::EXPOSURE_MASK |
    Gdk::BUTTON_PRESS_MASK |
    Gdk::BUTTON_RELEASE_MASK |
    Gdk::BUTTON1_MOTION_MASK;
  set_events( mask );

  Glib::RefPtr<Gdk::GL::Config> glConfig;
  glConfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB | Gdk::GL::MODE_DOUBLE);

  if( !glConfig )
    std::cerr << "Couldn't initialize glConfig!";

  if( !set_gl_capability(glConfig) || !is_gl_capable() )
    std::cerr << "Couldn't set_gl_capability to glconfig, or not gl_capable!";
 
}

double FrameDrawingArea::SnapValue( double coord ){
  return floor(coord) + 0.5;
}


// Also copied from FlyCapture
void FrameDrawingArea::InitializeOpenGL(){

  BeginGL();

  const char *extentions = (const char*)glGetString(GL_EXTENSIONS);
  if( strstr( extentions, "GL_ARB_pixel_buffer_object" ) != NULL){
    // This case was only handled in win32
  }

  InitializeImageTexture();

  glShadeModel( GL_FLAT );

  // Initialize matrices
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( 0, 1, 0, 1 );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  EndGL();

}

bool FrameDrawingArea::BeginGL(){

  if( !get_gl_window()->gl_begin( get_gl_context() ) ){
    return false;
  }

  return true;

}

void FrameDrawingArea::EndGL(){
  get_gl_window()->gl_end();
}

void FrameDrawingArea::DestroyImageTexture(){
  glDeleteTextures( sk_maxNumTextures, m_imageTextures);
}
