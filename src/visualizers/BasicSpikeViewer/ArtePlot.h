#ifndef ARTE_PLOT_H
#define ARTE_PLOT_H

#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif
#include "ArteAxes.h"

class ArtePlot: public ArteUIElement{
	
	char *p plotTitle;
	stl::list<ArteAxes> axesList;
	
public:
	ArtePlot();
	ArtePlot(int x, int y,int w,int h, char *n);
	void redraw();
	void setTitle(char *n);
	void setEnabled(bool enabled);
	void addAxes(int axesType);
}


#endif
