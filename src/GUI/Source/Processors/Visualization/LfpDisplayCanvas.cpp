/*
  ==============================================================================

    LfpDisplayCanvas.cpp
    Created: 8 Feb 2012 1:06:53pm
    Author:  jsiegle

  ==============================================================================
*/

#include "LfpDisplayCanvas.h"

LfpDisplayCanvas::LfpDisplayCanvas(AudioSampleBuffer* asb, MidiBuffer* mb, Configuration* config, LfpDisplayEditor* ed)
	 : xBuffer(10), yBuffer(10), editor(ed), displayBuffer(asb), eventBuffer(mb),
	    plotHeight(60), selectedChan(-1), screenBufferIndex(0)
{

	GenericProcessor* gp = (GenericProcessor*) editor->getProcessor();

	nChans = gp->getNumInputs();

	screenBuffer = new AudioSampleBuffer(nChans, getWidth());

	//setBounds(0,0,700,400);

	totalHeight = (plotHeight+yBuffer)*nChans + yBuffer;
	
}

LfpDisplayCanvas::~LfpDisplayCanvas()
{
}


void LfpDisplayCanvas::newOpenGLContextCreated()
{

	setUp2DCanvas();
	activateAntiAliasing();

	glClearColor (0.667, 0.698, 0.9, 1.0);
	resized();

}

void LfpDisplayCanvas::updateScreenBuffer()
{
	// copy new samples from the displayBuffer into the screenBuffer

}

void LfpDisplayCanvas::renderOpenGL()
{
	
	glClear(GL_COLOR_BUFFER_BIT); // clear buffers to preset values

	//drawTicks();

	updateScreenBuffer();

	for (int i = 0; i < nChans; i++)
	{
		bool isSelected = false;

		if (selectedChan == i)
			isSelected = true;

		if (checkBounds(i)) {
			setViewport(i);
			drawBorder(isSelected);
			drawChannelInfo(i,isSelected);
			//drawWaveform(i,isSelected);
		}	
	}
	drawScrollBars();
}

void LfpDisplayCanvas::drawWaveform(int chan, bool isSelected)
{
	// draw the screen buffer for a given channel
}


void LfpDisplayCanvas::drawTicks()
{
	
	glViewport(0,0,getWidth(),getHeight());

	glColor4f(1.0f, 1.0f, 1.0f, 0.25f);

	for (int i = 0; i < 10; i++)
	{
		if (i == 5)
			glLineWidth(3.0);
		else if (i == 1 || i == 3 || i == 7 || i == 9)
			glLineWidth(2.0);
		else
			glLineWidth(1.0);

		glBegin(GL_LINE_STRIP);
		glVertex2f(0.1*i,0);
		glVertex2f(0.1*i,1);
		glEnd();
	}
}


bool LfpDisplayCanvas::checkBounds(int chan)
{
	bool isVisible;

	int lowerBound = (chan+1)*(plotHeight+yBuffer);
	int upperBound = chan*(plotHeight+yBuffer);

	if (getScrollAmount() < lowerBound && getScrollAmount() + getHeight() > upperBound)
		isVisible = true;
	else
		isVisible = false;
	
	return isVisible;

}

void LfpDisplayCanvas::setViewport(int chan)
{
	glViewport(xBuffer,
			   getHeight()-(chan+1)*(plotHeight+yBuffer)+getScrollAmount(),
	           getWidth()-2*xBuffer,
	           plotHeight);
}

void LfpDisplayCanvas::drawBorder(bool isSelected)
{
	float alpha = 0.5f;

	if (isSelected)
		alpha = 1.0f;

	glColor4f(0.0f, 0.0f, 0.0f, alpha);
	glBegin(GL_LINE_STRIP);
 	glVertex2f(0.0f, 0.0f);
 	glVertex2f(1.0f, 0.0f);
 	glVertex2f(1.0f, 1.0f);
 	glVertex2f(0.0f, 1.0f);
 	glVertex2f(0.0f, 0.0f);
 	glEnd();

}

void LfpDisplayCanvas::drawChannelInfo(int chan, bool isSelected)
{
	float alpha = 0.5f;

	if (isSelected)
		alpha = 1.0f;

	glColor4f(0.0f,0.0f,0.0f,alpha);
	glRasterPos2f(5.0f/getWidth(),0.3);
	String s = String("Channel ");
	s += (chan+1);

	getFont(String("miso-regular"))->FaceSize(16);
	getFont(String("miso-regular"))->Render(s);
}

int LfpDisplayCanvas::getTotalHeight() 
{
	return totalHeight;
}


void LfpDisplayCanvas::mouseDown(const MouseEvent& e) 
{

	Point<int> pos = e.getPosition();
	int xcoord = pos.getX();

	if (xcoord < getWidth()-getScrollBarWidth())
	{
		int chan = (e.getMouseDownY() + getScrollAmount())/(yBuffer+plotHeight);

			selectedChan = chan;

		repaint();
	}

	mouseDownInCanvas(e);
}

void LfpDisplayCanvas::mouseDrag(const MouseEvent& e) {mouseDragInCanvas(e);}
void LfpDisplayCanvas::mouseMove(const MouseEvent& e) {mouseMoveInCanvas(e);}
void LfpDisplayCanvas::mouseUp(const MouseEvent& e) 	{mouseUpInCanvas(e);}
void LfpDisplayCanvas::mouseWheelMove(const MouseEvent& e, float a, float b) {mouseWheelMoveInCanvas(e,a,b);}

void LfpDisplayCanvas::resized()
{
	//screenBuffer = new AudioSampleBuffer(nChans, getWidth());



	canvasWasResized();
}