#ifndef PLOT_UTILS_H_
#define PLOT_UTILS_H_

#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif

#include <cstring>

void drawString(float x, float y, void *f, char *string);
void drawViewportEdge();


#endif