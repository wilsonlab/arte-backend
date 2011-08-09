#ifndef SIMPLE_SPIKE_PLOT_H_
#define SIMPLE_SPIKE_PLOT_H_

#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <pthread.h>
#include <set>
#include <map>

#include "netcom.h"
#include "datapacket.h"


const double MAX_VOLT = pow(2,15);
const int MAX_INT = pow(2,16);

// ===================================
// 		GUI Specific Variables
// ===================================

static char windowTitle[200] = {0};
static int winPosX = 5, winPosY = 20;
static int winWidth = 624, winHeight = 312;
static double commandWinHeight = 20; // Shift the entire UI up by commandWinHeight and reserve this area for text and buttons

static double xBox = winWidth/4;
static double yBox = winHeight/2;

static double xPadding = 2;
static double yPadding = 2;

static double const waveformLineWidth = 1;
static bool disableWaveOverlay = true;
static char txtDispBuff[40];

void *font = GLUT_BITMAP_8_BY_13;
static int IDLE_SLEEP_USEC = (1e6)/100;

// ===================================
// 		Scaling Variables
// ===================================
static int xRange = MAX_INT;
static int yRange = MAX_ING;

static double xScale = 2.0/(double)xRange;
static double yScale = 2.0/(double)yRange;

static float dV = 1.0/((float)MAX_VOLT*2);
static float userScale = 1;
static float dUserScale = .3;

static float voltShift = -.85;
static float userShift = 0;
static float dUserShift = .05;

static float const colWave[3] = {1.0, 1.0, 0.6};
static float const colThres[3] = {1.0, 0.1, 0.1};
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
static int nChan=4;
static int nProj=6;

static int const MAX_SPIKE_NET_BUFF_SIZE = 500;
static spike_net_t spikeBuff[MAX_SPIKE_NET_BUFF_SIZE];
static spike_net_t spike;
static int nSpikes = 0;
static int readIdx = 0;
static int writeIdx = 0;

// ===================================
// 		Spike Waveform specific plotting variables
// ===================================

static int const MAX_N_SAMPS_PER_WAVE = 320;
static GLint waves[4][MAX_N_SAMPS_PER_WAVE] = {0};
static int nSampsPerWave = 32;
// ===================================
// 		Inline Functions
// ===================================

// ===================================
// 		Misc Variables
// ===================================

static int totalSpikesRead =0;
static timeval startTime, now;

static int const cmdStrLen = 50;
static unsigned char cmd[cmdStrLen];
static int cIdx = 0;
// ===================================
// 		
// ===================================

void initNetworkRxThread();
void checkForNewSpikes();

// ===================================
// 		General Functions
// ===================================
int incrementIdx(int i);
bool tryToGetSpike(spike_net_t *s);

void idleFn();
void refreshDrawing();
void drawBoundingBoxes();
void eraseWaveforms();
void eraseCommandString();

void setViewportForWaveN(int n);
void setViewportForProjectionN(int n);
void setViewportForCommandString();

void drawViewportEdge();

void drawWaveforms();
void drawWaveformN(int n);
void drawSelectedWaveform();

void drawProjections();
void drawProjectionN(int n, int idx);
int calcWaveMaxInd();

void resizeWindow(int w, int h);

float scaleVoltage(int v, bool);

/*
// ===================================
// 		Keyboard & Command Function Headers
// ===================================

void toggleOverlay();
void clearWindow();

void keyPressedFn(unsigned char key, int x, int y);
void specialKeyFn(int key, int x, int y);
void enterCommandStr(char key);
void dispCommandString();

bool executeCommand(unsigned char * cmd);

void drawString(float x, float y, char *string);
void *getNetSpike(void *ptr);







// ===================================
// 		Command Variables
// ===================================
static int const CMD_CLEAR_WIN = 'c';
static int const CMD_TOGGLE_OVERLAY = 'o';
static int const CMD_SCALE_UP = '=';
static int const CMD_SCALE_DOWN = '-';
static int const CMD_SHIFT_UP = '+';
static int const CMD_SHIFT_DOWN = '_';

static int const CMD_STR_MAX_LEN = 16;
static int const CMD_THOLD_ALL = 'T';
static int const CMD_THOLD_SINGLE = 't';
static int const CMD_GAIN_ALL = 'G';
static int const CMD_GAIN_SINGLE = 'g';
static int const CMD_SET_POST_SAMPS = 'N';
static int const CMD_NULL = 0;
static int currentCommand = 0;
static bool enteringCommand = false;
static int selectedWaveform = 0;
*/
// ===================================
// Keyboard & Command Function Headers
// ===================================

void initCommandListAndMap();
void keyPressedFn(unsigned char key, int x, int y);
void specialKeyFn(int key, int x, int y);

void enterCommandArg(char key);
void dispCommandString();

bool executeCommand(char * cmd);

void clearWindow();
void resetSeqNum();
void quit();
void scaleUp();
void scaleDown();
void shiftUp();
void shiftDown();
void resetScale();
void nextColor();
void prevColor();
void showHelp();

void setGainAll(void*);
void setGainSingle(void*);
void setWindowTimeLen(void*);
void setFrameRate(void*);
void setChannelLabel(void*);

void setPortNumber(void*);
void setWindowXPos(void*);
void setWindowYPos(void*);
void setWindowHeight(void*);
void setWindowWidth(void*);
void setChannelLabel(void*);

void setPortNum(char *p);
bool updateWinLen(double l);
void updateFrameRate(int rate);

void drawString(float x, float y, char *string);

#endif

