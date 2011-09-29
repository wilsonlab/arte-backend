#ifndef TETRODE_PLOT_H_
#define TETRODE_PLOT_H_

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
//#include "viewerCommandDefs.h"


class TetrodePlot{
	
	static const double MAX_VOLT = 32768;//pow(2,15);
	static const int MAX_INT = 65536;//pow(2,16);

	// ===================================
	// 		GUI Specific Variables
	// ===================================

	char plotTitle[200];
	static const int titleHeight = 25;
	int xPos , yPos;
	int plotWidth, plotHeight;

	double xBox, yBox;

	double xPadding, yPadding;
	int plotPadX, plotPadY;

	double waveformLineWidth;
	bool disableWaveOverlay;
	
	void *font;
	
	// ===================================
	// 		Scaling Variables
	// ===================================
	int xRange, yRange;

	double xScale, yScale;

	float dV;
	float userScale;
	float dUserScale;

	float voltShift;
	float userShift;
	float dUserShift;

	float colSelected[3];// = {0.2, 0.2, 0.2};
	float colWave[3];// = {1.0, 1.0, 0.6};
	float colThres[3];// = {1.0, 0.1, 0.1};

	// ===================================
	// 		Network Variables
	// ===================================

	char host[11];// = "127.0.0.1";
	char * port;
	NetComDat net; // = NetCom::initUdpRx(host,port);

	// ===================================
	// 		Data Variables
	// ===================================
	
	int nChan, nProj;

	static int const MAX_SPIKE_BUFF_SIZE = 50;
	spike_net_t spikeBuff[MAX_SPIKE_BUFF_SIZE];
	spike_net_t spike;
	int nSpikes;
	int readIdx;
	int writeIdx;
	uint32_t curSeqNum;

	// ===================================
	// 		Spike Waveform specific plotting variables
	// ===================================

	static int const MAX_N_SAMPS_PER_WAVE = 320;
	GLint waves[4][MAX_N_SAMPS_PER_WAVE];;
	int nSampsPerWave;

	// ===================================
	// 		Misc Variables
	// ===================================

	int totalSpikesRead;
	timeval startTime, now;

	int cIdx;

	// ===================================
	// 		Network Function Headers
	// ===================================

	void initNetworkRxThread();
	void checkForNewSpikes();

	// ===================================
	// 		General Functions
	// ===================================
	int incrementIdx(int i);
	bool tryToGetSpikeForPlotting(spike_net_t *s);

	// ===================================
	// 		General Functions
	// ===================================
	void resetSeqNum();
	
	// ===================================
	// 		Plotting and Drawing Functions
	// ===================================
	void eraseWaveforms();
	void highlightSelectedWaveform();
	void drawWaveforms();
	void drawWaveformN(int n);
	void drawProjections();
	void drawProjectionN(int n, int idx);
	void drawTitle();
	void drawBoundingBoxes();


	void setViewportForTitleBox();
	void setViewportForWaveN(int n);
	void setViewportForProjectionN(int n);
	void setViewportForCommandString();
	void drawViewportEdge();

	int calcWaveMaxInd();

	float scaleVoltage(int v, bool);

	void drawString(float x, float y, char *string);
	
public:
	TetrodePlot();
	TetrodePlot(int x, int y, int w, int h, char* port);
	void draw();
	void resizePlot(int w, int h);
	void movePlot(int x, int y);
	
	void getNetworkSpikePacket();
};

static float const colSelected[3] = {0.2, 0.2, 0.2};
static float const colWave[3] = {1.0, 1.0, 0.6};
static float const colThres[3] = {1.0, 0.1, 0.1};

void* networkThreadFunc(void* arg);

#endif
