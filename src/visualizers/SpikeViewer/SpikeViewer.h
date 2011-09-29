#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif

#include "TetrodePlot.h"

static const int MAX_N_PLOT = 64;
TetrodePlot plots[MAX_N_PLOT];
static int nPlots=0;

void drawTetrodePlots();
void idleFunc();
void resizeWinFunc(int w, int h);
void initPlots();
