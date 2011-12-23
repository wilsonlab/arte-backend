#include "ArteUIElement.h"

ArteUIElement::ArteUIElement():
xpos(0),  ypos(0), width(100), height(100), enabled(true), padding(0)
{	
	elementName = (char*) "ArteUIElement";
	clearNextDraw = false;
}

ArteUIElement::ArteUIElement(int x, int y, double w, double h):
enabled(true), padding(0)
{
	xpos = x+padding;
	ypos = y+padding;
	width = w-padding*2;
	height = h-padding*2;
	elementName = (char*) "ArteUIElement";	
	clearNextDraw = false;
}
ArteUIElement::ArteUIElement(int x, int y, double w, double h, int p):
enabled(true), padding(0)
{
	xpos = x+padding;
	ypos = y+padding;	
	width = w-padding*2;
	height = h-padding*2;
	elementName = (char*) "ArteUIElement";	
	clearNextDraw = false;
}

void ArteUIElement::redraw(){
//	std::cout<<"ArteUIELement::redraw(), Position:"<<xpos<<","<<ypos<<" : "<<width<<","<<height<<std::endl;
	setGlViewport();
	
	if (clearNextDraw){
		clearNextDraw = false;
		glColor3f(0.0, 0.0, 0.0);
		glRecti(-1,-1,1,1);
		glutSwapBuffers();
		glRecti(-1,-1,1,1);
		glutSwapBuffers();
	}
}
void ArteUIElement::drawElementEdges(){
	// std::cout<<"ArteUIELement::drawElementEdges(), Position:"<<xpos<<","<<ypos<<" : "<<width<<","<<height<<std::endl;
	glColor3f(1.0, 1.0, 1.0);
	setGlViewport();
	glLineWidth(2);
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
void ArteUIElement::clearOnNextDraw(bool c){
	clearNextDraw = c;
}

bool ArteUIElement::hitTest(int x, int y){
    return (x>xpos && x<xpos+width) && (y>ypos && y<ypos+height);
}
