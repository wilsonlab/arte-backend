#include "frame_drawing_area.h"
#include <GL/glu.h>

FrameDrawingArea::FrameDrawingArea(multi_frame_collections_map *m){
  frames_map = m;
  //ready = false;
  //set_size_request(80,80);
  //show();
}

FrameDrawingArea::FrameDrawingArea(BaseObjectType* cobject, 
                                   const Glib::RefPtr<Gtk::Builder>& refGlade){
  ready = false;
  set_size_request(80,80);
  show();
}

FrameDrawingArea::~FrameDrawingArea(){
  DestroyImageTexture();
}

bool FrameDrawingArea::on_expose_event(GdkEventExpose *event){

  //  std::lock_guard<std::mutex> lk(m_imageMutex);

  double validTextureWidth = 1.0;
  double validTextureHeight = 1.0;
  GLenum errorno;

  assembleView();

  BeginGL();

  glEnable( GL_TEXTURE_2D );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  // draw the image

  glBindTexture(GL_TEXTURE_2D, m_imageTextures[0]);

  glTexImage2D( GL_TEXTURE_2D,
                0,
                GL_LUMINANCE,
                width, height,
                0,
                GL_LUMINANCE,
                GL_UNSIGNED_BYTE,
                (unsigned char *) unsigned_imageData );


  if (get_gl_window()->is_double_buffered()){
    get_gl_window()->swap_buffers();
  } else {
    glFlush();
  }

  errorno = glGetError();
  if(errorno != GL_NO_ERROR){
    std::cerr << "There was an OPENGL error drawing the image!\n";
  }

  // One-texture case (the only case I'm handling

  glBegin( GL_QUADS );

  glTexCoord2d(0.0, validTextureHeight);
  glVertex2d(0.0, 0.0);

  glTexCoord2d(validTextureWidth, validTextureHeight);
  glVertex2d(1.0, 0.0);

  glTexCoord2d(validTextureWidth, 0.0);
  glVertex2d(1.0, 1.0);

  glTexCoord2d(0.0, 0.0);
  glVertex2d(0.0, 1.0);

  glEnd();
  return true;

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
bool FrameDrawingArea::CommonInit(TrackerOpt& o, multi_frame_collections_map *f){

  my_opt = o;
  the_frames = f;

  fresh_frame = false; // not used?  Checking not implemented.

  all_selected = false;
  selection = NULL;
  map_selection = "background_subtracted";
    map_selection = "test";
  //map_selection = "raw";
  //  map_selection = "background";

  // Display is: Main screen, with max_n_cams little screens underneath

  scale_down_to_fraction = 1.0 / (double) my_opt.max_n_cams();

  height = FRAME_HEIGHT + (int)((float)FRAME_HEIGHT * scale_down_to_fraction);
  width  = FRAME_WIDTH;

  reference_frame = cvCreateImage( cvSize(height,width),
                                   IPL_DEPTH_8S,
                                   1);
  signed_imageData = reference_frame->imageData;

  reference_frame2 = cvCreateImage( cvSize(height,width),
                                    IPL_DEPTH_8U,
                                    1);
  unsigned_imageData = reference_frame2->imageData;

  Gtk::GL::init(0, NULL);


  set_size_request(width,height);

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

  bool rv = set_gl_capability(glConfig);
  if( !rv || !is_gl_capable() )
    std::cerr << "Couldn't set_gl_capability to glconfig, or not gl_capable!";



  // tracker.cpp decides when 'ready'
  //  ready = true;
  show();

  return true;

}

double FrameDrawingArea::SnapValue( double coord ){
  return floor(coord) + 0.5;
}


// Also copied from FlyCapture
void FrameDrawingArea::InitializeOpenGL(){

  std::cout << "About to InitializeOpenGL\n";
  if(true) {
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
    ready = true;

  } else {
    std::cerr << "While trying to InitializeOpenGL, got a BeginGL error\n";
  }
  std::cout << "Finished initializeOpenGL\n";

}

void FrameDrawingArea::on_realize(){
  std::cout << "ON_REALIZE\n";
  Gtk::GL::DrawingArea::on_realize();
  InitializeOpenGL();
  std::cout << "Finished ON_REALIZE\n";
}

bool FrameDrawingArea::on_motion_notify_event( GdkEventMotion* event ){
  std::cout << "Motion Notify Event\n";
}

bool FrameDrawingArea::on_button_release_event( GdkEventButton* event){
  std::cout << "Button Release Event\n";
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


// Copied.  I'm not sure what this does! :/
// TODO: read up on opengl
bool FrameDrawingArea::on_configure_event(GdkEventConfigure* event){

  if(true){

    BeginGL();
    int screenWidth = 0;
    int screenHeight = 0;

    // This line also copied from ImageDrawingArea.  Seems wrong
    get_window()->get_size(screenWidth, screenHeight);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0,0,screenWidth,screenHeight);
    //glViewport(0,0,width,height);

    EndGL();

    return true;
  } else {
    std::cerr << "While trying to on_configure_event got a BeginGL error.\n";
  }

}

void FrameDrawingArea::my_blit_to_view( char*source,
                                        int sourceWidth, int sourceHeight,
                                        int source_pix_bytes,
                                        int dest_x, int dest_y,
                                        int scale_down){

  // Very specific to the layout I have in mind!
  // There is one big frames at view top,
  // and max_cams frames at bottom

  if(source_pix_bytes > 1){
    std::cerr << "Tried to blit with image greater "
              << "than one byte per pixel.  Not supported "
              << "by this rudimentary version of 'blit'"
              << std::endl;
  }

  if(scale_down == 1){ // the big frame,
    if(dest_x > 0 || dest_y > 0){
      std::cerr << "Tried to blit the big frame to something "
                << "other than the upper-left corner.  Not "
                << "allowed in this rudimentary version of 'blit'"
                << std::endl;
    }
    // just copy the source into the front of the view's array
    memcpy(unsigned_imageData, source,
           (sourceWidth*sourceHeight*source_pix_bytes));

  } else {

    // Deal with the scaled-down case
    for(int y = 0;
        y < (sourceHeight/scale_down)
          && (y + dest_y ) < height

          ;
        y++){

      for(int x = 0; x < (sourceWidth/scale_down) &&
            (x + dest_x ) < width;
          x++){

        unsigned_imageData[ (y+dest_y)*width + //row
                   (x+dest_x) //col
                   ] =

          source[ y*sourceWidth*scale_down + //row
                  (x*scale_down) //col
                  ];
      } // column loop
    } // row loop
  } // done with scaled_down image

}

void FrameDrawingArea::set_selection(ArteFrame* new_sel, bool all){
  selection = new_sel;
  all_selected = all;
}

void FrameDrawingArea::set_map_selection(std::string& new_sel){
  map_selection = new_sel;
}

void FrameDrawingArea::assembleView(){

  // Grab selection, or the first camera from first
  // camera group from selected frames set (raw, bkgnd_sub, or hybr]
  ArteFrame *this_sel =
    (selection == NULL)?
    (*frames_map)[ map_selection ][0][0] : selection;

  //  std::cout << "map_selection: " << map_selection << std::endl;

  int depth = (this_sel->depth == IPL_DEPTH_8U
               || this_sel->depth == IPL_DEPTH_8S) ?
    1 : 2;

  // Do the big image
  my_blit_to_view( this_sel->imageData,
                   this_sel->width, this_sel->height,
                   depth,
                   0, 0,
                   1);

  // Do all the sub images
  frame_collections *v = &((*frames_map)[map_selection]);
  int c_i = 0;
  frame_collections::iterator g;
  frame_p_collection::iterator c;
  for(g = v->begin(); g != (v->end()); g++){
    for(c = g->begin(); c != g->end(); c++){

      int x = c_i * ((*c)->width/my_opt.max_n_cams());
      int y = (*c)->height+1;


      my_blit_to_view( (*c)->imageData,
                       (*c)->width, (*c)->height,
                       depth,
                       x,
                       y,
                       my_opt.max_n_cams());

      c_i++; //count off one camera done on bottom
    }

  }

  // convert to unsigned
  //  for(int p = 0; p < height * width; p++){
  //    unsigned_imageData[p] = (unsigned char)signed_imageData[p];
  //  }

}
