#include "ArteTitleBox.h"
#include "PlotUtils.h"

ArteTitleBox::ArteTitleBox():
ArteUIElement(0,0,15,100,1)
{
	setColor(.15, .15, .50);
	setSelectedColor(.15, .50, .15);
	selected = false;
	std::cout<<"ArteTitleBox() regular constructor"<<std::endl;
	ArteUIElement::elementName = (char*) "ArteTitleBox - Un initialized";
}

ArteTitleBox::ArteTitleBox(int x, int y,int w,int h, char *n):
ArteUIElement(x,y,w,h,1)
{
	setColor(.15, .15, .50);
	setSelectedColor(.15, .50, .15);
	selected = false;
	ArteUIElement::elementName = (char*) "ArteTitleBox";

	std::cout<<"ArteTitleBox() custom constructor X,Y  W,H"<<x<<","<<y<<" - "<<w<<","<<h<<std::endl;
}

void ArteTitleBox::redraw(){
	ArteUIElement::redraw();
	

	if(selected)
			glColor3fv(selectedColor);
		else
			glColor3fv(titleColor);

	// draw the colored background for the plot
	glRecti(-1,-1,1,1);

	// Reset color to white so we can draw the title text in white
	glColor3f(1.0, 1.0, 1.0);
	//
	// Draw title Text here
	//
//	std::cout<<ArteUIElement::elementName<<":"<<ArteUIElement::xpos<<","<<ArteUIElement::ypos<<" HxW:"<<ArteUIElement::width<<","<<ArteUIElement::height<<std::endl;
	ArteUIElement::drawElementEdges();
}
void ArteTitleBox::setTitle(char *n){
	title = n;
}
void ArteTitleBox::setSelected(bool sel){
	selected = sel;
}

void ArteTitleBox::setColor(GLfloat r, GLfloat g, GLfloat b){
	titleColor[0] = r;
	titleColor[1] = g;
	titleColor[2] = b;
}
void ArteTitleBox::setSelectedColor(GLfloat r, GLfloat g, GLfloat b){
	selectedColor[0] = r;
	selectedColor[1] = g;
	selectedColor[2] = b;
}