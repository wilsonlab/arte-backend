#ifndef ARTE_AXES_H
#define ARTE_AXES_H

#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif
#include <stdlib.h>

#include "ArteUIElement.h"
#include "datapacket.h"
#include "PlotUtils.h"

 	
class ArteAxes: public ArteUIElement{
	int type;
	void plotWaveform(int c);
	void plotProjection(int p);
    double xlims[2];
	double ylims[2];
	spike_net_t s;
	GLfloat waveColor[3];
	GLfloat thresholdColor[3];
	GLfloat pointColor[3];
	GLfloat gridColor[3];
	
	void calcWaveformPeakIdx(int,int,int*,int*);
	
	bool gotFirstSpike;
	bool resizedFlag;
	
  	void drawWaveformGrid(int thold, int gain);
	void drawProjectionGrid(int gain1, int gain2);

public:
	ArteAxes();
	ArteAxes(int x, int y, double w, double h, int t);
	void plotData();
	void updateSpikeData(spike_net_t s);
	void setXLims(double xmin, double xmax);
	void getXLims(double *xmin, double *xmax);
	void setYLims(double ymin, double ymax);
	void getYLims(double *ymin, double *ymax);
	void setType(int type);
	int getType();
    
	void redraw();
	
	void setWaveformColor(GLfloat r, GLfloat g, GLfloat b);
	void setThresholdColor(GLfloat r, GLfloat g, GLfloat b);
	void setPointColor(GLfloat r, GLfloat g, GLfloat b);
	void setGridColor(GLfloat, GLfloat, GLfloat);
	
	void setPosition(int,int,double,double);
   
	
	bool drawWaveformLine;
	bool drawWaveformPoints;
	bool overlay;
	bool drawGrid;
	bool convertLabelUnits;
	
	void clearOnNextDraw(bool c);
};



#endif
