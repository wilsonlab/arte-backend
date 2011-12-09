#include "ArteUIElement.h"

ArteUIElement::ArteUIElement():
xpos(0),  ypos(0), width(100), height(100), enabled(true), padding(0)
{	
	elementName = (char*) "ArteUIElement";
}

ArteUIElement::ArteUIElement(int x, int y, double w, double h):
enabled(true), padding(0)
{
	xpos = x+padding;
	ypos = y+padding;
	width = w-padding*2;
	height = h-padding*2;
	elementName = (char*) "ArteUIElement";	
}
ArteUIElement::ArteUIElement(int x, int y, double w, double h, int p):
enabled(true), padding(p)
{
	xpos = x+padding;
	ypos = y+padding;
	width = w-padding*2;
	height = h-padding*2;
	elementName = (char*) "ArteUIElement";	
}

void ArteUIElement::redraw(){
//	std::cout<<"ArteUIELement::redraw(), Position:"<<xpos<<","<<ypos<<" : "<<width<<","<<height<<std::endl;
	setGlViewport();
}
void ArteUIElement::drawElementEdges(){
	// std::cout<<"ArteUIELement::drawElementEdges(), Position:"<<xpos<<","<<ypos<<" : "<<width<<","<<height<<std::endl;
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
	glLoadIdentity();
	// std::cout<<xpos<<"x"<<ypos<<"-"<<width<<"x"<<height<<"\t"<<elementName<<std::endl;
}
void ArteUIElement::setPosition(int x, int y, double w, double h){
	xpos = x+padding;
	ypos = y+padding;
	width = w - padding*2;
	height = h - padding*2;
}
