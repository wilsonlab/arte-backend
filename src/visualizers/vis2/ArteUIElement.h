#ifndef ARTE_UI_ELEMENT_
#define ARTE_UI_ELEMENT_

#include "PlotUtils.h"

class ArteUIElement{
	
	int xpos, ypos;
	double height, width;
	bool enabled;
	
public:
	ArteUIElement();
	ArteUIElement(int x, int y, double w, double h);
	void redraw();
	void drawBoundingEdges();
	void setEnabled(bool e);
	bool getEnabled();
}

#endif
