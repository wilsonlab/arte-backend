#ifndef ARTE_UI_ELEMENT_
#define ARTE_UI_ELEMENT_

#include "PlotUtils.h"

class ArteUIElement{

protected:	
	int xpos, ypos;
	double height, width;
	bool enabled;
	double padding;
	
	void setGlViewport();
		
public:
	ArteUIElement();
	ArteUIElement(int x, int y, double w, double h);
	void redraw();
	void drawElementEdges();
	void setEnabled(bool e);
	bool getEnabled();
	void setPos(int x, int y, double w, double h);
};



#endif
