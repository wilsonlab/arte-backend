//
//  OpenGLComponent.cpp
//  jSpike
//
//  Created by Stuart Layton on 12/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef ArteOpenGLComponent_cpp
#define ArteOpenGLComponent_cpp

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>

#include "JuceHeader.h"
#include "ArteTetrodePlot.h"
#include "TetrodeSource.h"

#if JUCE_OPENGL

    #if JUCE_WINDOWS
        #include <gl/gl.h>
        #include <gl/glu.h>
    #elif JUCE_LINUX
        #include <GL/gl.h>
        #include <GL/glut.h>
        #undef KeyPress
    #elif JUCE_IOS
        #include <OpenGLES/ES1/gl.h>
        #include <OpenGLES/ES1/glext.h>
    #elif JUCE_MAC
        #include <GLUT/glut.h>
    #elif JUCE_IOS
        //#include <GL/glut.h>
    #endif

    #ifndef GL_BGRA_EXT
        #define GL_BGRA_EXT 0x80e1
    #endif

#define kPlotRefreshRate 30

//==============================================================================
class OpenGLCanvas  : public OpenGLComponent,
public Timer
{
public:
    OpenGLCanvas()
    : count (0),
    glFlagsSet(false),
    clearFlag(true),
    resizeFlag(true)
    {
        
        
    }
    
    ~OpenGLCanvas()
    {
        
    }
    
    void initialize(char *p){
        startTimer (1000/kPlotRefreshRate);
        port = p;
        ts = TetrodeSource(port);
        tp = ArteTetrodePlot(0,0,300, 300, "ArteTetrodePlot");
        tp.setDataSource(ts);
        tp.setTitleEnabled(false);
		tp.setEnabled(false);
		tp.initAxes();
    }
    
    // when the component creates a new internal context, this is called, and
    // we'll use the opportunity to create the textures needed.
    void newOpenGLContextCreated()
    {
    }
    void mouseDown(const MouseEvent& e){
        std::cout<<"YLocation:"<< e.getMouseDownY()<<" Corrected:"<<getHeight() - e.getMouseDownY()<<std::endl;
        int x = e.getMouseDownX();
        int y = getHeight() - e.getMouseDownY();
        std::cout<<"Mouse Down Event:"<<x<<","<<y<<std::endl;
        tp.mouseDown(x,y);
        const Point<int> p = e.getPosition();

        prevMouseX = x;
        prevMouseY = y;
        
    }
    void mouseDrag (const MouseEvent& e)
    {
        const Point<int> p = e.getPosition();
        
        int x = p.getX();
        int y = getHeight() - p.getY();
        
        int dx = x - prevMouseX;
        int dy = y - prevMouseY;
    
        if (dx!=0)
            tp.mouseDragX(dx);
        if (dy!=0)
            tp.mouseDragY(dy);
        
        prevMouseX = x;
        prevMouseY = y;
    }
    
    void renderOpenGL()
    {
     
        if(clearFlag){
            glClearColor (0.0, 0.0, 0.0, 0.0);
            glClear (GL_COLOR_BUFFER_BIT);
            clearFlag = false;
        }
        if (!glFlagsSet)
            setGLFlags();

        if(!tp.getEnabled()){
            
            TetrodeSource * ts = tp.getDataSource();
			ts->enableSource();
			tp.setEnabled(true);
        }
        if(resizeFlag){
            tp.setPosition(0,0,getWidth(), getHeight());
            resizeFlag = false;
        }
        tp.redraw();
    }
    
    void timerCallback()
    {
        count++;
        repaint();
    }
    
private:
    int count;
    ArteTetrodePlot tp;
    TetrodeSource ts;
    char * port;
    
    bool glFlagsSet;
    bool clearFlag;
    bool resizeFlag;
    
    int prevMouseX;
    int prevMouseY;
    
    void setGLFlags(){
        glDisable(GL_DITHER);
        glDisable(GL_POINT_SMOOTH);
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_POLYGON_SMOOTH);
        glHint(GL_POINT_SMOOTH, GL_DONT_CARE);
        glHint(GL_LINE_SMOOTH, GL_DONT_CARE);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
        glDisable( GL_MULTISAMPLE_ARB) ;
        glFlagsSet = true;
    }
};


//==============================================================================
class ArteOpenGLComponent  : public Component
{
public:
    //==============================================================================
    ArteOpenGLComponent(char *p)
    {
        setName ("OpenGL");
        canvas.initialize(p);
        addAndMakeVisible (&canvas);
    }
    
    void resized()
    {
        canvas.setBounds (0, 0, getWidth(), getHeight());
    }
    
private:
    OpenGLCanvas canvas;
    
    ArteOpenGLComponent (const ArteOpenGLComponent&);
    ArteOpenGLComponent& operator= (const ArteOpenGLComponent&);
};


//==============================================================================
//Component* createArteOpenGLComponent()
//{
//    return new ArteOpenGLComponent();
//}


#endif
#endif