#include "ArteUIElement.h"

ArteUIElement::ArteUIElement():
xpos(0),  ypos(0), width(100), height(100), enabled(true), padding(1)
{	
}

ArteUIElement::ArteUIElement(int x, int y, double w, double h):
enabled(true), padding(1)
{
	xpos = x+padding;
	ypos = y+padding;
	width = w-padding*2;
	height = h-padding*2;
	
}

void ArteUIElement::redraw(){
//	std::cout<<"ArteUIELement::redraw(), Position:"<<xpos<<","<<ypos<<" : "<<width<<","<<height<<std::endl;
	setGlViewport();
}
void ArteUIElement::drawElementEdges(){
	setGlViewport();
	drawViewportEdge();
}
void ArteUIElement::setEnabled(bool e){
	enabled = e;	
}
bool ArteUIElement::getEnabled(){
	return enabled;
}
void ArteUIElement::setGlViewport(){
	glViewport(xpos, ypos, width, height);
}
void ArteUIElement::setPos(int x, int y, double w, double h){
	xpos = x+padding;
	ypos = y+padding;
	width = w - padding*2;
	height = h - padding*2;
}
