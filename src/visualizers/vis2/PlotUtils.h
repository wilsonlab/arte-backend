#ifndef PLOT_UTILS_H_
#define PLOT_UTILS_H_


#if defined(__linux__)
	#include <GL/glut.h>
#else // assume OS X
	#include <GLUT/glut.h>
	#include <OpenGL/glu.h>
	#include <OpenGL/glext.h>
#endif

#include <cstring>
#include <iostream>

void checkGlError();
void setViewportWithRange(int x,int y,int w, int h,int xMin,int xMax,int yMin,int yMax);
void drawString(float x, float y, void *f, char *string);
void strokeString(void*f, char *string);
void drawViewportEdge();

void drawViewportCross(int xMin, int yMin, int xMax, int yMax);


#endif