#ifndef ARTE_AXES_H
#define ARTE_AXES_H

#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif

#include "ArteUIElement.h"

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
	plotWaveform(int c);
	plotProjection(int p);
	double xlims[2];
	double ylims[2];
	spike_net_t* s;
	GLfloat waveColor[3];
	GLfloat thresholdColor[3];
	GLfloat projectionColor[3];
	
	int calcWaveformPeakIdx();
	
public:
	ArteAxes();
	ArteAxes(int x, int y, int w, int h, int t);
	void plotData();
	void updateSpikeData(spike_net_t *s);
	void setXLims(double xmin, xmax);
	void setYLims(double ymin, ymax);
	void setType(int type);
	
	void setWaveformColor(GLfloat r, GLfloat g, GLfloat b);
	void setThresholdColor(GLfloat r, GLfloat g, GLfloat b);
	void setProjectionColor(GLfloat r, GLfloat g, GLfloat b);
	
}



#endif
