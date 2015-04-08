#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h> // Include the GLEW header file

#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
	#include <OpenGL/glu.h>
	#include <OpenGL/glext.h>
#endif
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <getopt.h>
#include "TetrodePlot.h"
#include "PlotUtils.h"
#include "SpikeViewer.h"


void redraw();
void idleFunc();
void resizeWinFunc(int w, int h);
void initPlots(int c, int r);

void keyPressedFn(unsigned char key, int x, int y);
void specialKeyFn(int key, int x, int y);
void mouseClickFn(int button, int state, int x, int y);

