#ifndef GLUT_WINDOW_H_
#define GLUT_WINDOW_H_

#define GL_GLEXT_PROTOTYPES
#include <glew.h> // Include the GLEW header file

#if defined(__linux__)
	#include <GL/glut.h>
	#include <GL/glu.h>
	#include <GL/glext.h>
#else // assume OS X
	#include <GLUT/glut.h>
	#include <OpenGL/glu.h>
	#include <OpenGL/glext.h>
#endif

#include <list>
#include <iostream>
#include <cmath>

#include "ArteTetrodePlot.h"

static int x,y,w,h;
static int nRow;
static int nCol;
static char** ports;
static int sleepTime = 1e6/60;
static int nPlots;

static bool windowPositioned = false;
static bool windowSized = false; 
static bool portsSpecified = false;
static bool layoutDimsSpecified = false;
static bool clearNextDraw = false;


static std::list<ArteTetrodePlot> tetrodePlots;
static std::list<TetrodeSource> tetrodeSources;
//

void positionTetrodePlots();
void createGlutWindow( int x, int  y, int w, int h, char *title, int argc, char *argv[]);	
void resizeCallback(int w, int h);
void keyPressedCallback(unsigned char key, int x, int y);

void specialKeyCallback(int key, int x, int y);
void mouseClickCallback(int button, int state, int x, int y);
void mouseEventCallback(int button, int state, int x, int y);

void idleCallback();
void displayCallback();

void positionWindow(int, int);
void resizeWindow(int,int);

void setPorts(char *ports[]);
void setLayoutDims(int c, int r);
void showWindow();

void calcGridSize();

void clearDoubleBuffer();
void clearSingleBuffer();
void clearPlots();


	

#endif
