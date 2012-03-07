#ifndef ARTE_WINDOW_H
#define ARTE_WINDOW_H

#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif

class ArteWindow{
	
	int xpos,ypos,width,height;
	
	double redrawRate;
	
	char *p windowtitle;
	
	stl::list plotList;
	
public:
	ArteWindow();
	ArteWindow(int x, int y,int w,int h, char *n);
	void setRedrawRate(double rate);
	void redraw();
	void setTitle(char *n);
	void removePlot(int i);
	void addPlot(int i);
	ArtePlot getPlot(int i);
}


#endif
