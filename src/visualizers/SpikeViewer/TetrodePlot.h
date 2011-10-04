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
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <stdlib.h>

#include <assert.h>

#include "netcom.h"
#include "datapacket.h"
#include "PlotUtils.h"
//#include "viewerCommandDefs.h"


class TetrodePlot{
	
	static const double MAX_VOLT = 32768;//pow(2,15);
	static const int MAX_INT = 65536;//pow(2,16);

	// ===================================
	// 		GUI Specific Variables
	// ===================================

	char plotTitle[200];
	int tetrodeNumber;
	static const int titleHeight = 22;
	int xPos, yPos;
	int plotWidth, plotHeight;

	double xBox, yBox;	


	double waveformLineWidth;
	bool disableWaveOverlay;
	
	void *font;
	void *titleFont;
	
	bool isSelected;
	
	bool clearProjectionNextPlot;
	
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
	float colFont[3];
	float colTitle[3];
	float colTitleSelected[3];
	char txtDispBuff[40];

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

	void checkForNewSpikes();

	// ===================================
	// 		General Functions
	// ===================================
//	int incrementIdx(int i);
	bool tryToGetSpikeForPlotting(spike_net_t *s);

	// ===================================
	// 		General Functions
	// ===================================
	void resetSeqNum();
	
	// ===================================
	// 		Plotting and Drawing Functions
	// ===================================
	void initColors();
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
//	void drawViewportEdge();

	int calcWaveMaxInd();

	float scaleVoltage(int v, bool);

//	void drawString(float x, float y, void *f, char *string);
	
	int incrementIdx(int i);
	
public:
	double padLeft, padRight, padTop, padBottom;
	
	TetrodePlot();
	TetrodePlot(int x, int y, int w, int h, char* port);
	void draw();
	void resizePlot(int w, int h);
	void movePlot(int x, int y);
	
	void initNetworkRxThread();
	void getNetworkSpikePacket();
	
	void setSelected(bool s);
	bool isPlotSelected();
	
	void scaleUp();
	void scaleDown();
	void setScale(float s);
	float getScale();
	
	void shiftUp();
	void shiftDown();
	void setShift(float s);
	float getShift();
	
	void setWaveformOverlay(bool o);
	bool toggleWaveformOverlay();
	bool getWaveformOverlay();
	
	void clearPlot();	
	
	int getTetrodeNumber();
	void setTetrodeNumber(int);
	
	bool containsPoint(int x, int y);
	
	int getMaxX();
	int getMinX();
	
	// implement scaleUp(), scaleDown(), setScale(), getScale()
	// shiftUp(), shiftDown(), setShift(), getShift()
	// clearProjections()
	// toggleOverlay()
};


void* networkThreadFunc(void* arg);

#endif
