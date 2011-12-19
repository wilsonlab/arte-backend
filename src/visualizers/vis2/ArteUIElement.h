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
	ArteUIElement(int x, int y, double w, double h, int p);
	void redraw();
	void drawElementEdges();
	void setEnabled(bool e);
	bool getEnabled();
	void setPosition(int x, int y, double w, double h);
	
	char *elementName;
	void clearOnNextDraw(bool);
	bool clearNextDraw;
	
};



#endif
