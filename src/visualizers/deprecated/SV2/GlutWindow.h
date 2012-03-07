#ifndef GLUT_WINDOW_H_
#define GLUT_WINDOW_H_

#define GL_GLEXT_PROTOTYPES


#if defined(__linux__)
	#include <GL/glew.h> // Include the GLEW header file
	#include <GL/glut.h>
	#include <GL/glu.h>
	#include <GL/glext.h>
#else // assume OS X
	#include <glew.h> // Include the GLEW header file		
	#include <GLUT/glut.h>
	#include <OpenGL/glu.h>
	#include <OpenGL/glext.h>
#endif


#include "glIncludes.h"
#include <iostream>
#include "PlotCollection.h"

static int x,y,w,h;
static int nRow;
static int nCol;
static char** ports;
static int sleepTime = 1e6/100;

static bool windowPositioned = false;
static bool windowSized = false; 
static bool portsSpecified = false;
static bool layoutDimsSpecified = false;

//

void createGlutWindow( int x, int  y, int w, int h, char *title, int argc, char *argv[]);	
void resizeCallback(int w, int h);
void keyPressedCallback(unsigned char key, int x, int y);

void specialKeyCallback(int key, int x, int y);
void mouseClickCallback(int button, int state, int x, int y);

void idleCallback();
void displayCallback();

void positionWindow(int, int);
void resizeWindow(int,int);

void setPorts(char *ports[]);
void setLayoutDims(int c, int r);
void showWindow();



	

#endif
