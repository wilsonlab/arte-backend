#ifndef ARTE_AXES_H
#define ARTE_AXES_H

#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif

#include "ArteUIElement.h"
#include "datapacket.h"

#define WAVE1 0
#define WAVE2 1
#define WAVE3 2
#define WAVE4 3
#define PROJ1x2 4
#define PROJ1x3 5
#define PROJ1x4 6
#define PROJ2x3 7
#define PROJ2x4 8
#define PROJ3x4 9
 	
class ArteAxes: public ArteUIElement{
	int type;
	void plotWaveform(int c);
	void plotProjection(int p);
	double xlims[2];
	double ylims[2];
	spike_net_t* s;
	GLfloat waveColor[3];
	GLfloat thresholdColor[3];
	GLfloat pointColor[3];
	
	int calcWaveformPeakIdx();
	
	
public:
	ArteAxes();
	ArteAxes(int x, int y, double w, double h, int t);
	void plotData();
	void updateSpikeData(spike_net_t *s);
	void setXLims(double xmin, double xmax);
	void setYLims(double ymin, double ymax);
	void setType(int type);
	
	void redraw();
	
	void setWaveformColor(GLfloat r, GLfloat g, GLfloat b);
	void setThresholdColor(GLfloat r, GLfloat g, GLfloat b);
	void setPointColor(GLfloat r, GLfloat g, GLfloat b);
	
	void setPosition(int,int,double,double);
	
	bool drawWaveformLine;
	bool drawWaveformPoints;
};



#endif
