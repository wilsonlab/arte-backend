#ifndef ARTE_PLOT_H
#define ARTE_PLOT_H

#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif
#include <list>
#include <math.h>

#include "ArteAxes.h"
#include "ArteTitleBox.h"
#include "ArteUIElement.h"
#include "TetrodeSource.h"
#include "PlotUtils.h"


class ArteTetrodePlot : public ArteUIElement{
	
	char *plotTitle;
	std::list<ArteAxes> axesList;
	ArteTitleBox titleBox;
	TetrodeSource tetSource;
	bool enabled;
    bool enableTitle;
	void drawTitle();
	
	double titleHeight;
	void *ptr;
    
    bool limitsChanged;
    double limits[4][2];
    
    ArteAxes* selectedAxes;
    int selectedAxesN;
    
    void zoomAxes(int n, bool xdim, bool zoomin);
    void zoomProjection (int n, bool xdim, bool zoomin);
    void zoomWaveform (int n, bool xdim, bool zoomin);
    
    void initLimits();
	
public:
	ArteTetrodePlot();
	ArteTetrodePlot(int x, int y,int w,int h, char *n);
	void initAxes();
	void redraw();
	void setTitle(char *n);
	void setEnabled(bool enabled);
	bool getEnabled();
	void setDataSource(TetrodeSource tp);
	void setPosition(int,int,double,double);
	TetrodeSource *getDataSource();
	int getNumberOfAxes();
	void clearOnNextDraw(bool c);
    void setTitleEnabled(bool e);
    
    void mouseDown(int x, int y);
    
    void mouseDragX(int dx);
    void mouseDragY(int dy);

};



#endif
