/*
  ==============================================================================

    LfpDisplayNode.cpp
    Created: 8 Feb 2012 12:31:32pm
    Author:  jsiegle

  ==============================================================================
*/

#include "LfpDisplayNode.h"
#include <stdio.h>

LfpDisplayNode::LfpDisplayNode()
	: GenericProcessor("LFP Viewer"),
	  timebase(1000), displayGain(1), parameterChanged(true), inWindow(false),
	  xBuffer(10), yBuffer(10), 
	  plotHeight(60), selectedChan(-1)

{
	setNumInputs(16);
	setSampleRate(25000.0);
	setPlayConfigDetails(16,0,44100.0,128);
	//setSampleRate(25000.0);
	//numInputs = 16;
	//totalHeight = (plotHeight+yBuffer)*nChans + yBuffer;

	//resizeBuffer();
	displayBuffer = 0;
	//displayBuffer = new AudioSampleBuffer(250000,16);
	eventBuffer = new MidiBuffer();
}

LfpDisplayNode::~LfpDisplayNode()
{
	if (displayBuffer != 0)
		deleteAndZero(displayBuffer);

	deleteAndZero(eventBuffer);
}



AudioProcessorEditor* LfpDisplayNode::createEditor()
{

	std::cout << "Processor data viewport: " << getDataViewport() << std::endl;

	LfpDisplayEditor* editor = new LfpDisplayEditor(this, viewport, getDataViewport());

	editor->setBuffers(displayBuffer,eventBuffer);
	editor->setUIComponent(getUIComponent());
	editor->setConfiguration(config);

	setEditor(editor);
	
	std::cout << "Creating LFP Display Editor." << std::endl;
	return editor;

}

void LfpDisplayNode::setNumInputs(int inputs)
{
	numInputs = inputs;	
	setNumOutputs(0);

	int nSamples = (int) sampleRate*10.0f;
	int nInputs = getNumInputs();
	std::cout << "Setting inputs. Samples: " << nSamples << ", Inputs: " << nInputs << std::endl;

	setPlayConfigDetails(getNumInputs(), 0, 44100.0, 128);
	//resizeBuffer();
}

void LfpDisplayNode::setSampleRate(float r)
{
	sampleRate = r;
	int nSamples = (int) sampleRate*10.0f;
	int nInputs = getNumInputs();
	std::cout << "Setting sample rate. Samples: " << nSamples << ", Inputs: " << nInputs << std::endl;

	//resizeBuffer();
}

void LfpDisplayNode::resizeBuffer()
{
	int nSamples = (int) sampleRate*10.0f;
	int nInputs = getNumInputs();

	std::cout << "Resizing buffer. Samples: " << nSamples << ", Inputs: " << nInputs << std::endl;

	//displayBuffer = new AudioSampleBuffer(nInputs, nSamples);

}

void LfpDisplayNode::setParameter (int parameterIndex, float newValue)
{
	//std::cout << "Message received." << std::endl;

	if (parameterIndex == 0) {
		timebase = newValue;
	} else {
		displayGain = newValue;
	}

	parameterChanged = true;

}

void LfpDisplayNode::prepareToPlay (double, int)
{
	resizeBuffer();
}


void LfpDisplayNode::process(AudioSampleBuffer &buffer, MidiBuffer &midiMessages, int& nSamples)
{
	// 1. place any new samples into the displayBuffer

	// 2. indicate number of new samples in the eventBuffer

	// 3. if any parameters have changed, place more events in the eventBuffer
	
}


void LfpDisplayNode::newOpenGLContextCreated()
{

	setUp2DCanvas();
	activateAntiAliasing();

	glClearColor (0.8, 0.4, 0.9, 1.0);
	resized();

}

void LfpDisplayNode::renderOpenGL()
{
	
	glClear(GL_COLOR_BUFFER_BIT); // clear buffers to preset values

	//drawTicks();

	//updateScreenBuffer();

	for (int i = 0; i < getNumInputs(); i++)
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

void LfpDisplayNode::drawWaveform(int chan, bool isSelected)
{
	// draw the screen buffer for a given channel
}


void LfpDisplayNode::drawTicks()
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


bool LfpDisplayNode::checkBounds(int chan)
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

void LfpDisplayNode::setViewport(int chan)
{
	glViewport(xBuffer,
			   getHeight()-(chan+1)*(plotHeight+yBuffer)+getScrollAmount(),
	           getWidth()-2*xBuffer,
	           plotHeight);
}

void LfpDisplayNode::drawBorder(bool isSelected)
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

void LfpDisplayNode::drawChannelInfo(int chan, bool isSelected)
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

int LfpDisplayNode::getTotalHeight() 
{
	return (plotHeight+yBuffer)*getNumInputs() + yBuffer;
}


void LfpDisplayNode::resized()

{

	canvasWasResized();
	// glClear(GL_COLOR_BUFFER_BIT);

	// int h, w;

	// if (inWindow)
	// {
	// 	h = getParentComponent()->getHeight();
	// 	w = getParentComponent()->getWidth();
	// } else {
	// 	h = getHeight();
	// 	w = getWidth();
	// }

	// if (getScrollAmount() + h > getTotalHeight() && getTotalHeight() > h)
	// 	setScrollAmount(getTotalHeight() - h);
	// else
	// 	setScrollAmount(0);

	// showScrollBars();

}
