#ifndef SIMPLE_SPIKE_PLOT_H_
#define SIMPLE_SPIKE_PLOT_H_

#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif

#include <iostream>
#include <map>
#include <math.h>
#include <pthread.h>
#include <set>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "netcom.h"
#include "datapacket.h"
#include "viewerCommandDefs.h"

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
static int yRange = MAX_INT;

static double xScale = 2.0/(double)xRange;
static double yScale = 2.0/(double)yRange;

static float dV = 1.0/((float)MAX_VOLT*2);
static float userScale = 1;
static float dUserScale = .3;

static float voltShift = -.85;
static float userShift = 0;
static float dUserShift = .05;

static float const colSelected[3] = {0.2, 0.2, 0.2};
static float const colWave[3] = {1.0, 1.0, 0.6};
static float const colThres[3] = {1.0, 0.1, 0.1};
static float const colFont[3] = {1.0, 1.0, 1.0};
// ===================================
// 		Network Variables
// ===================================

static char *portarray[32]={};
static int startingport;
static char portchar[6];
static char host[] = "127.0.0.1";
static NetComDat netarray[32]={};
NetComDat net; // = NetCom::initUdpRx(host,port);

// ===================================
// 		Data Variables
// ===================================
static int nChan=4;
static int nProj=6;

static int const MAX_SPIKE_BUFF_SIZE = 300;
static spike_net_t spikeBuff[MAX_SPIKE_BUFF_SIZE];
static spike_net_t spike;
static int nSpikes = 0;
static int readIdx = 0;
static int writeIdx = 0;
static uint32_t curSeqNum = 0;

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

static int cIdx = 0;
// ===================================
// 		Network Function Headers
// ===================================

void initNetworkRxThread();
void checkForNewSpikes();

// ===================================
// 		Signal Functions
// ===================================
void signalUser1(int param);
void signalUser2(int param);

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
void highlightSelectedWaveform();

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

#endif
