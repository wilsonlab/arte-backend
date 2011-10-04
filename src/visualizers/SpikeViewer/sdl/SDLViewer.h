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
static char app_name[] = "Arte Network Spike Viewer v0.9 - (C) 2011 Stuart Layton, MIT";

static const int MAX_N_PLOT = 64;

TetrodePlot *plots[MAX_N_PLOT];

static int nPlots = 0;
static int selectedPlot = 0;

static bool allOverlay = true;

void drawTetrodePlots();
void idleFunc();
void resizeWinFunc(int w, int h);
void initPlots(int c, int r);

static void quit(int code);
static void handle_key_down( SDL_keysym* keysym );
static void process_events( void );
static void draw_screen( void );
static void setup_opengl( int width, int height );

// ===================================
// 		Command Variables
// ===================================

// Application Title
void drawAppTitle();
void setViewportForTitle();

// Command Functions
void scaleUpAll();
void scaleUpSel();
void scaleDownAll();
void scaleDownSel();
void shiftUpAll();
void shiftUpSel();
void shiftDownAll();
void shiftDownSel();
void toggleOverlayAll();
void toggleOverlaySel();
void clearAll();
void clearSel();
void showHelp();
void quit();

void initCommandListAndMap();

// Command Engine Map Declaration
void drawCommandString();
void setViewportForCommandWin();

static int const cmdStrLen = 500;
static char cmd[cmdStrLen];
static int cmdStrIdx = 0;
static int cmdWinHeight=22;
static int cmdWinWidth = 100;
static float cmdWinCol[] = {.2, .2, .2};

typedef void cmdfunc_t(void);
typedef void cmdfunc2_t(void*);
typedef std::map<char, cmdfunc_t*> cmdfnmap_t;
typedef std::map<char, cmdfunc2_t*> cmdfnmap2_t;

static cmdfnmap_t quickCmdMap;
static std::set<int> slowCmdSet;
static cmdfnmap2_t slowCmdMap;

// Command Engine State Variables
static int const CMD_STATE_QUICK = 0; // state where quick commands get executed
static int const CMD_STATE_SLOW = 1;  // state where user specifies what command is to be executed
static int const CMD_STATE_SLOW_ARG = 2;// state where user enters in the command argument
static int cmdState = CMD_STATE_QUICK;

// Simple commands that respond immediately
static int const CMD_CLEAR_ALL 	= 'C';
static int const CMD_CLEAR_SEL  = 'c';
static int const CMD_SCALE_UP_SEL 	= '=';
static int const CMD_SCALE_DOWN_SEL = '-';
static int const CMD_SCALE_UP_ALL 	= '+';
static int const CMD_SCALE_DOWN_ALL = '_';
static int const CMD_RESET_SCALE    = 'R';
static int const CMD_SHIFT_UP_SEL 	= ']';
static int const CMD_SHIFT_DOWN_SEL = '[';
static int const CMD_SHIFT_UP_ALL 	= '}';
static int const CMD_SHIFT_DOWN_ALL = '{';
static int const CMD_OVERLAY_SEL 	= 'o';
static int const CMD_OVERLAY_ALL	= 'O';

static int const CMD_HELP 		= '?';

// Commands that require CTRL + KEY
static int const CMD_QUIT = 17;
static int const CMD_RESET_SEQ = 18;

// Commands that require an input string
static int const CMD_STR_MAX_LEN 	= 16; 
static int const CMD_GAIN_ALL 		= 'g';
static int const CMD_GAIN_SINGLE 	= 'G';
static int const CMD_THOLD_ALL 		= 'T';
static int const CMD_THOLD_SINGLE 	= 't';
static int const CMD_NULL 			=  0;
static int const CMD_SET_WIN_LEN 	= 'L';
static int const CMD_SET_FRAMERATE 	= 'R';
static int const CMD_SET_POST_SAMPS = 'S';

static int const KEY_DEL = 127;
static int const KEY_BKSP = 8;
static int const KEY_ENTER = 13;

static int currentCommand = 0;
static bool enteringCommand = false;

// ===================================
// 		options parsing 
// ===================================

static const char CMD_SET_WIN_NAME 	= 'n';
static const char CMD_SET_WIN_POSX	= 'x';
static const char CMD_SET_WIN_POSY	= 'y';
static const char CMD_SET_WIN_W		= 'w';
static const char CMD_SET_WIN_H		= 'h';

static struct option long_options[] =
{
	{"windowname",required_argument, 0, CMD_SET_WIN_NAME},
	{"xposition", required_argument, 0, CMD_SET_WIN_POSX},
	{"yposition", required_argument, 0, CMD_SET_WIN_POSY},
	{"width",     required_argument, 0, CMD_SET_WIN_W},
	{"height",    required_argument, 0, CMD_SET_WIN_H},
	{"framerate", required_argument, 0, CMD_SET_FRAMERATE},
	{0, 0, 0, 0}
};

void parseCommandLineArgs(int argc, char**argv);

