#include "ArteAxes.h"

ArteAxes::ArteAxes():
ArteUIElement(),
type(1),
drawWaveformLine(true),
drawWaveformPoints(false),
drawGrid(true),
gotFirstSpike(false),
overlay(false)
{	
	ylims[0] = 0;
	ylims[1] = 1;
	setWaveformColor(1.0,1.0,0.6);
	setThresholdColor(1.0, 0.1, 0.1);
	setPointColor(1.0, 1.0, 1.0);
	setGridColor(0.4, 0.2, 0.2);
	
	ArteUIElement::elementName = (char*) "ArteAxes";
}

ArteAxes::ArteAxes(int x, int y, double w, double h, int t):
ArteUIElement(x,y,w,h),
drawWaveformLine(true),
drawWaveformPoints(false),
drawGrid(true),
gotFirstSpike(false),
overlay(false)
{
	// if (t<WAVE1 || t>PROJ3x4)
		//("Invalid Axes type specified");
	type = t;
	setWaveformColor(1.0,1.0,0.6);
	setThresholdColor(1.0, 0.1, 0.1);
	setPointColor(1.0, 1.0, 1.0);
	setGridColor(0.15, 0.15, 0.15);
	ArteUIElement::elementName = (char*) "ArteAxes";

}
void ArteAxes::updateSpikeData(spike_net_t newSpike){
	if (!gotFirstSpike){
		gotFirstSpike = true;
		printf("ArteAxes::updateSpikeData() got first spike\n");
	}
	s = newSpike;
}
void ArteAxes::redraw(){
	ArteUIElement::redraw();
	if (ArteUIElement::enabled)
		plotData();

	ArteUIElement::drawElementEdges();
}
void ArteAxes::plotData(){

	if (!gotFirstSpike)
	{
		std::cout<<"\tWaiting for the first spike"<<std::endl;
		return;
	}
	switch(type){
		case WAVE1:
		case WAVE2:
		case WAVE3:
		case WAVE4:
		plotWaveform(type);
		break;
		
		case PROJ1x2:
		case PROJ1x3:
		case PROJ1x4:
		case PROJ2x3:
		case PROJ2x4:
		case PROJ3x4:
		plotProjection(type);
		break;
		default:
			std::cout<<"ArteAxes::plotData(), Invalid type specified, cannot plot"<<std::endl;
			exit(1);
	}
}
void ArteAxes::setYLims(double ymin, double ymax){
	ylims[0] = ymin;
	ylims[1] = ymax;
}
void ArteAxes::getYLims(double *min, double *max){
	*min = ylims[0];
	*max = ylims[1];
}
void ArteAxes::setType(int t){
	 if (t<WAVE1 || t>PROJ3x4){
		std::cout<<"Invalid Axes type specified";
		return;
	}
	
	type = t;
}


void ArteAxes::plotWaveform(int chan){
	
	if (drawGrid)
		drawWaveformGrid();
	if (chan>WAVE4 || chan<WAVE1)
	{
		std::cout<<"ArteAxes::plotWaveform() invalid channel, must be between 0 and 4"<<std::endl;
		return;
	}
	
	if (s.n_samps_per_chan>1024)
		return;
		
	// Set the plotting range for the current axes 
	// xdims are 0->number of samples per waveform minus one so the line goes all the way to the edges
	// ydims are specified by the ylims vector		
	setViewportRange(0, ylims[0], s.n_samps_per_chan-1, ylims[1]);
	

	if(!overlay){
		glColor3f(0.0,0.0,0.0);
		glRectd(0,ylims[0], s.n_samps_per_chan, ylims[1]);
	}
	if(drawGrid)
		drawWaveformGrid();
	
	//compute the spatial width for each wawveform sample	
	float dx = 1;
	float x = 0;
	int	sampIdx = chan; 

	//Draw the individual waveform points connected with a line
	// if drawWaveformPoints is set to false then force the drawing of the line, _SOMETHING_ must be drawn
	glColor3fv(waveColor);
	
	//if drawWaveformLine and drawWaveformPoints are both set
	if(drawWaveformLine){
		glLineWidth(1);
		glBegin( GL_LINE_STRIP );
		for (int i=0; i<s.n_samps_per_chan; i++)
		{
			glVertex2f(x, s.data[sampIdx]);
			sampIdx +=4;
			x +=dx; 
		}
		glEnd();
	}
	

	//if drawWaveformLine and drawWaveformPoints are both set false then draw the points
	//this ensures that something is always drawn
	if(drawWaveformPoints || !drawWaveformLine){
		x = 0;
		sampIdx = chan;
		glColor3fv(pointColor);
		glPointSize(1);
		glBegin( GL_POINTS );
		for (int i=0; i<s.n_samps_per_chan; i++)
		{
			glVertex2f(x, s.data[sampIdx]);
			sampIdx +=4;
			x +=dx;
		}
		glEnd();
	}
	// Draw the threshold line and label
	int thresh = s.thresh[chan];
	
	glColor3fv(thresholdColor);
	glLineWidth(1); 
	glLineStipple(4, 0xAAAA); // make a dashed line
	glEnable(GL_LINE_STIPPLE);

	glBegin( GL_LINE_STRIP );
		glVertex2f(0, thresh);
		glVertex2f(s.n_samps_per_chan, thresh);
	glEnd();		

	glDisable(GL_LINE_STIPPLE);

	char str[100] = {0};
	sprintf(str, "%d", (int) thresh);
	float yOffset = (ylims[1] - ylims[0])/ArteUIElement::height * 2;
	drawString(1 ,thresh + yOffset, GLUT_BITMAP_8_BY_13, str);
}


void ArteAxes::plotProjection(int proj){
//	std::cout<<"ArteAxes::plotProjection():"<<proj<<" not yet implemented"<<std::endl;
	// if (proj<PROJ1x2 || proj>PROJ3x4)
		// error("ArteAxes:plotProjection() invalid projection specified");
	
	setViewportRange(ylims[0], ylims[0], ylims[1], ylims[1]);
	drawViewportCross();
	int d1, d2;
	if (proj==PROJ1x2){
		d1 = 0;
		d2 = 1;
	}
	else if(proj==PROJ1x3){
		d1 = 0;
		d2 = 2;
	}
	else if(proj==PROJ1x4){
		d1 = 0;
		d2 = 3;
	}
	else if(proj==PROJ2x3){
		d1 = 1;
		d2 = 2;
	}
	else if(proj==PROJ2x4){
		d1 = 1;
		d2 = 3;
	}
	else if (proj==PROJ3x4){
		d1 = 2;
		d2 = 3;
	}
	else{
		std::cout<<"ArteAxes::plotProjection() invalid projection specified cannot determine d1 and d2"<<std::endl;
		return;
	}
	
	int maxIdx = calcWaveformPeakIdx();
//	std::cout<<"MaxIDX:"<<maxIdx<<std::endl;
	if (drawGrid)
		drawProjectionGrid();
	glColor3fv(pointColor);
	glPointSize(1);
	glBegin(GL_POINTS);
		glVertex2f(s.data[maxIdx+d1], s.data[maxIdx+d2]);
	glEnd();
	
}

int ArteAxes::calcWaveformPeakIdx(){
//Calculate which sample in the waveform across all channels has the highest peak voltage 
//and then calculate its sample number

	int idx = -1;
	int val = -1*2^15;
	for (int i=0; i<s.n_samps_per_chan * s.n_chans; i++)
		if(val < s.data[i])
		{
			idx = i;
			val = s.data[i];
		}
	// The index of the peak voltage can be any of the channels so shift it back to the first channel
	idx = idx - idx%s.n_chans;
	return idx;
}
void ArteAxes::drawWaveformGrid(){

	double voltRange = ylims[1] - ylims[0];
	double pixelRange = ArteUIElement::height;
	//This is a totally arbitrary value that i'll mess around with and set as a macro when I figure out a value I like
	int minPixelsPerTick = 25;

	int nTicks = pixelRange / minPixelsPerTick;
	int voltPerTick = (voltRange / nTicks);
	// Round to the nearest 200

	
	double meanRange = voltRange/2;
	glColor3fv(gridColor);

	glLineWidth(1);
	char str[100] = {0};
	for (int i=0; i<nTicks; i++){
		// Draw the individual ticks
		double tickVoltage = roundUp(ylims[0] + voltPerTick/4 + (i * voltPerTick), 200);
		// if the tick is too close to the top of the axes don't draw it.
	
		glBegin(GL_LINE_STRIP);
		glVertex2i(0, tickVoltage);
		glVertex2i(s.n_samps_per_chan, tickVoltage);
		glEnd();
	
		// Write the voltage level
		sprintf(str, "%d", (int) tickVoltage);
//		str = itoa(tickVoltage, )
		drawString(1, tickVoltage+voltPerTick/10, GLUT_BITMAP_8_BY_13, str);
	}
}
void ArteAxes::drawProjectionGrid(){
	double voltRange = ylims[1] - ylims[0];
	double pixelRange = ArteUIElement::height;
	//This is a totally arbitrary value that i'll mess around with and set as a macro when I figure out a value I like
	int minPixelsPerTick = 25;

	int nTicks = pixelRange / minPixelsPerTick;
	int voltPerTick = (voltRange / nTicks);
	// Round to the nearest 200


	double meanRange = voltRange/2;
	glColor3fv(gridColor);

	glLineWidth(1);
	char str[100] = {0};
	for (int i=0; i<nTicks; i++){
		// Draw the individual ticks
		double tickVoltage = roundUp(ylims[0] + voltPerTick/4 + (i * voltPerTick), 200);
		// if the tick is too close to the top of the axes don't draw it.

		glBegin(GL_LINE_STRIP);
		glVertex2i(0, tickVoltage);
		glVertex2i(tickVoltage, tickVoltage);
		glVertex2i(tickVoltage, 0);
		glEnd();

		// Write the voltage level
		sprintf(str, "%d", (int) tickVoltage);
//		str = itoa(tickVoltage, )
		drawString(1, tickVoltage+voltPerTick/10, GLUT_BITMAP_8_BY_13, str);
	}
}
void ArteAxes::setWaveformColor(GLfloat r, GLfloat g, GLfloat b){
	waveColor[0] = r;
	waveColor[1] = g;
	waveColor[2] = b;
}
void ArteAxes::setThresholdColor(GLfloat r, GLfloat g, GLfloat b){
	thresholdColor[0] = r;
	thresholdColor[1] = g;
	thresholdColor[2] = b;
}
void ArteAxes::setPointColor(GLfloat r, GLfloat g, GLfloat b){
	pointColor[0] = r;
	pointColor[1] = g;
	pointColor[2] = b;
}
void ArteAxes::setGridColor(GLfloat r, GLfloat g, GLfloat b){
	gridColor[0] = r;
	gridColor[1] = g;
	gridColor[2] = b;
}
void ArteAxes::setPosition(int x, int y, double w, double h){
	ArteUIElement::setPosition(x,y,w,h);	
}