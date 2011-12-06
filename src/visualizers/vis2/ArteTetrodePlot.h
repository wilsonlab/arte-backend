#ifndef ARTE_PLOT_H
#define ARTE_PLOT_H

#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif
#include <list>

#include "ArteAxes.h"
#include "ArteTitleBox.h"
#include "ArteUIElement.h"
#include "TetrodeSource.h"


class ArteTetrodePlot : public ArteUIElement{
	
	char *plotTitle;
	std::list<ArteAxes> axesList;
	ArteTitleBox titleBox;
	TetrodeSource tetSource;
	bool enabled;
	void initAxes();
	void drawTitle();
	
	double titleHeight;
	
public:
	ArteTetrodePlot();
	ArteTetrodePlot(int x, int y,int w,int h, char *n);

	void redraw();

	void setTitle(char *n);
	void setEnabled(bool enabled);
	void setDataSource(TetrodeSource tp);
};


#endif
