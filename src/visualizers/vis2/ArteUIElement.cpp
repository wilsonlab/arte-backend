#include "ArteUIElement.h"

ArteUIElement::ArteUIElement():
x(0),  y(0), w(100), h(100), enabled(true)
{	
}

ArteUIElement::ArteUIElement(int x, int y, double w, double h):
enabled(true)
{
	xpos = x;
	ypos = y;
	width = w;
	height = h;
	
}

void ArteUIElement::redraw(){
	if (!enabled)
		return;
	drawBoundingEdges();
}
void ArteUIElement::drawBoundingEdges(){
	drawViewportEdge();
}
void ArteUIElement::setEnabled(bool e){
	enabled = e;	
}
bool ArteUIElement::getEnabled(){
	return e;
}