#ifndef SIMPLE_LFP_PLOT_H_

#if defined(__linux__)
	#include <GL/glut.h>
#else // assume OS X
	#include <GLUT/glut.h>
#endif

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <pthread.h>
#include <getopt.h>

#include "netcom.h"
#include "datapacket.h"

#define X_TO_PIX(x) ((x*winWidth)/MAX_INT)
#define PIX_TO_X(p) ((p*MAX_INT)/winWidth)

const double MAX_VOLT = pow(2,15);
const int MAX_INT = pow(2,16);
// ===================================
// 		GUI Specific Variables
// ===================================

static char windowTitle[200] = {0};
static int winPosX = 5, winPosY = 20;
static int winWidth = 1000, winHeight = 400;
static double commandWinHeight = 20;

static double xBox = 75;
static double yBox = winHeight/8;

static double xPadding = 1;
static double yPadding = 2;

static double const waveformLineWidth = 1;
static bool disableWaveOverlay = true;
static char txtDispBuff[40];

void *font = GLUT_BITMAP_8_BY_13;
static int IDLE_SLEEP_USEC = (1e6)/40;

// ===================================
// 		Scaling Variables
// ===================================
static int xRange = pow(2,16);
static int yRange = pow(2,16);
static double xScale = 2.0 / xRange;
static double yScale = 2.0 / (double)(yRange);

// Defines how much to shift the waveform within the viewport
//static float dV = 1.0/((float)MAX_VOLT*2);

static float dV = yRange/8;
static float userScale = 1;
static float dUserScale = .1;

static float voltShift = 0;
static float userShift = 0;
static float dUserShift = .01 * pow(2,16);

static float const colSelected[3] = {0.4, 0.4, 0.4};
static float const colFont[3] = {1.0, 1.0, 1.0};

// ===================================
// 		Network Variables
// ===================================

static char host[] = "127.0.0.1";
static char * port;
static NetComDat net; // = NetCom::initUdpRx(host,port);


// ===================================
// 		Data Variables
// ===================================
static int nChans=8;
static int const MAX_LFP_NET_BUFF_SIZE = 260;
static lfp_bank_net_t lfpBuff[MAX_LFP_NET_BUFF_SIZE];
static lfp_bank_net_t lfp;
static int nBuff = 0;
static uint64_t readInd = 0;
static uint64_t writeInd = 0;


// ===================================
// 		Plotting Variables
// ===================================
static uint32_t curSeqNum = 0;
static uint32_t prevSeqNum = 0;
static int nSampsPerChan = 2;
static double sampleRate = 2000;
static double winDt = 2;
static int maxIdx = winDt * sampleRate;

static uint64_t dIdx = 0;
//static uint64_t pIdx = 0;

static int nBuffLost = 0;
static int xPos = 0;
//static int dXPos = xRange/(maxIdx);

static const int MAX_N_CHAN = 8;
static const int MAX_N_SAMP = 32000;
static GLint waves[MAX_N_CHAN][MAX_N_SAMP*2]; //we need an x and y point for each sample
static int colWave[MAX_N_CHAN];

static const int CHAN_NAME_LEN = 7;
static char chName[MAX_N_CHAN][CHAN_NAME_LEN+1] = {0};
static char defName[] = "Tet N:M\0";

// ===================================
// 		Inline Functions
// ===================================

int inline IND(uint64_t i)	
{	
	return i%MAX_LFP_NET_BUFF_SIZE;	
}

int inline IDX(uint64_t i)	
{
		return i%maxIdx;	
}

inline GLint SCALE_VOLTAGE(int v, int chan){	
	return v * userScale / nChans + yRange - yRange/2/nChans - chan*yRange/nChans  + userShift;
}


// ===================================
// 		Misc Variables
// ===================================
static int totalBufsampleRateRead =0;
static timeval startTime, now;

static int const cmdStrLen = 50;
static char cmd[cmdStrLen];
static int cmdStrIdx = 0;

// ===================================
// 		Command Variables
// ===================================
// Simple commands that respond immediately
static int const CMD_CLEAR_WIN = 'c';
static int const CMD_TOGGLE_OVERLAY = 'o';
static int const CMD_SCALE_UP = '=';
static int const CMD_SCALE_DOWN = '-';
static int const CMD_SHIFT_UP = '+';
static int const CMD_SHIFT_DOWN = '_';
static int const CMD_RESET_SCALE = 'C';
static int const CMD_PREV_COL = '[';
static int const CMD_NEXT_COL = ']';

// Commands that require CTRL + KEY
static int const CMD_QUIT = 17;
static int const CMD_RESET_SEQ = 18;

// Commands that require an input string
static int const CMD_STR_MAX_LEN = 16; 
static int const CMD_GAIN_ALL = 'g';
static int const CMD_GAIN_SINGLE = 'G';
static int const CMD_NULL = 0;
static int const CMD_SET_WIN_LEN = 'L';
static int const CMD_SET_FRAMERATE = 'R';

static int const KEY_DEL = 127;
static int const KEY_BKSP = 8;
static int const KEY_ENTER = 13;

static int currentCommand = 0;
static bool enteringCommand = false;
static int selectedWaveform = 0;

// ===================================
// 		General Functions
// ===================================
int incrementIdx(int i);
bool tryToGetLfp(lfp_bank_net_t *s);
void updateNChans(int n);
void updateNSamps(int n);
void updateWaveArray();

void idleFn();
void redrawWindow();
void drawInfoBox();
void eraseOldWaveform();
void clearWaveforms();
void eraseCommandString();

void setViewportforWaveInfoN(int n);
void setViewportForWaves();
void setViewportForCommandString();
void setWaveformColor(int col);

void drawViewportEdge();

void drawWaveforms();
void drawWaveformN(int n);

void setWaveformColor(int c);
void highlightSelectedWaveform();

void resizeWindow(int w, int h);

int scaleVoltage(int v, int chan);
// ===================================
// 		Keyboard & Command Function Headers
// ===================================

void toggleOverlay();
void clearWindow();

void keyPressedFn(unsigned char key, int x, int y);
void specialKeyFn(int key, int x, int y);

void enterCommandStr(char key);
void dispCommandString();

bool executeCommand(char * cmd);

void setPortNum(char *p);
bool updateWinLen(double l);
void updateFrameRate(int rate);
void setChannelLabel(int n, char* s, int l);


void drawString(float x, float y, char *string);
void *readNetworkLfpData(void *ptr);

void initializeWaveVariables();
void loadWaveformColors();

// ===================================
// 		options parsing 
// ===================================

static const char ARG_WIN_NAME 	= 'n';
static const char ARG_WIN_POSX	= 'x';
static const char ARG_WIN_POSY	= 'y';
static const char ARG_WIN_W		= 'w';
static const char ARG_WIN_H		= 'h';
static const char ARG_PORT_NUM	= 'p';
static const char CH00	=  0;
static const char CH01	=  1;
static const char CH02	=  2;
static const char CH03	=  3;
static const char CH04	=  4;
static const char CH05	=  5;
static const char CH06	=  6;
static const char CH07	=  7;

static struct option long_options[] =
{
	{"windowname",required_argument, 0, ARG_WIN_NAME},
	{"xposition", required_argument, 0, ARG_WIN_POSY},
	{"yposition", required_argument, 0, ARG_WIN_POSX},
	{"width",     required_argument, 0, ARG_WIN_W},
	{"height",    required_argument, 0, ARG_WIN_H},
	{"port",	  required_argument, 0, ARG_PORT_NUM},
	{"framerate", required_argument, 0, CMD_SET_FRAMERATE},
	{"ch00label", required_argument, 0, CH00},
	{"ch01label", required_argument, 0, CH01},
	{"ch02label", required_argument, 0, CH02},
	{"ch03label", required_argument, 0, CH03},
	{"ch04label", required_argument, 0, CH04},
	{"ch05label", required_argument, 0, CH05},
	{"ch06label", required_argument, 0, CH06},
	{"ch07label", required_argument, 0, CH07},
	{0, 0, 0, 0}
};

void parseCommandLineArgs(int argc, char**argv);

#endif
