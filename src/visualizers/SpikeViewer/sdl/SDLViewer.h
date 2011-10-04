#if defined(__linux__)
	#include <GL/glut.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
#else
	#include <GLUT/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <getopt.h>
#include "TetrodePlot.h"

#include <SDL/SDL.h>


static const int MAX_N_PLOT = 64;

TetrodePlot *plots[MAX_N_PLOT];

static int nPlots = 0;
static int selectedPlot = 0;

static bool allOverlay = true;

static int IDLE_SLEEP_USEC = (1e6)/80;
static int winWidth = 1262;
static int winHeight = 762;

static int nCol = 4;
static int nRow = 2;

void drawTetrodePlots();
void idleFunc();
void resizeWinFunc(int w, int h);
void initPlots(int c, int r);

static void quit(int code);
static void handle_key_down( SDL_keysym* keysym );
static void process_events( void );
static void draw_screen( void );
static void setup_opengl( int width, int height );
