#include "ArteTetrodePlot.h"

ArteTetrodePlot::ArteTetrodePlot():
titleHeight(15){
	ArteUIElement();
	plotTitle = (char*) "ArteTetrodePlot";
	initAxes();
	//titleBox = ArteTitleBox(100-titleHeight,0,2,titleHeight, plotTitle);
	ArteUIElement::elementName = (char*) "ArtePlot";

	
}
ArteTetrodePlot::ArteTetrodePlot(int x, int y, int w, int h, char *n):
ArteUIElement(x,y,w,h), 
titleHeight(15){
	plotTitle = n;
	initAxes();
	titleBox = ArteTitleBox(x, y+h-titleHeight-3, w, titleHeight+3, plotTitle);
	// titleBox = ArteTitleBox(200, 50, 50, 10, plotTitle);
	std::cout<< x+h-titleHeight << "," << y << " - " << w << "," <<titleHeight<<std::endl;
	ArteUIElement::elementName = (char*) "ArtePlot";

}

void ArteTetrodePlot::setDataSource(TetrodeSource source){
	tetSource = source;
}
// Each plot needs to update its children axes when its redraw gets called. it also needs to call the parent plot
// when children axes get added it should place them in the correct location because it KNOWS where WAVE1 and PROJ1x3
// should go by default. This isn't as general as it should be but its a good push in the right direction
void ArteTetrodePlot::redraw(){
	// std::cout<<"ArteTetrodePlot() starting drawing"<<std::endl;
	ArteUIElement::redraw();

	std::list<ArteAxes>::iterator i;

	for (i=axesList.begin(); i!= axesList.end(); ++i){
		i->redraw();
	}
	titleBox.redraw();
	ArteUIElement::drawElementEdges();
	// std::cout<<"ArteTetrodePlot() Done drawing"<<std::endl;
}
void ArteTetrodePlot::setTitle(char *n){
	plotTitle = n;
}
void ArteTetrodePlot::setEnabled(bool e){
	ArteUIElement::enabled = e;
}
void ArteTetrodePlot::initAxes(){
	int minX = ArteUIElement::xpos;
	int minY = ArteUIElement::ypos;
	
	double axesWidth = ArteUIElement::width/4.0;
	double axesHeight = (ArteUIElement::height - titleHeight)/2.0;
	
	int axX, axY, axW, axH;
	
	if (!axesList.empty()){
		std::list<ArteAxes> tmp;
		axesList = tmp;
	}
	for (int i=WAVE1; i<=WAVE4; i++) // macro constants from ArteAxes.h
	{
		// divide axes width by 2 because wave plots are half of a projection plot width
		axX = minX  + axesWidth/2 * ( i % 2);
		axY = minY +  axesHeight * ( i / 2);
		ArteAxes ax = ArteAxes(axX, axY, axesWidth/2, axesHeight, i);
		ax.setEnabled(false);
		axesList.push_back(ax);
	}

	for (int i=PROJ1x2; i<=PROJ3x4; i++){
		// use i2 instead of i so we can index from 0 instead of 4
		int i2 = i - PROJ1x2;
		axX = minX + axesWidth *  (i2%3 + 1); // add 1 to offset for the waveform plots
		axY = minY + axesHeight * (i2 / 3); // no need to offset for the y direction

		ArteAxes ax = ArteAxes(axX, axY, axesWidth, axesHeight, i);
		ax.setEnabled(false);
		axesList.push_back(ax);
	}
}
