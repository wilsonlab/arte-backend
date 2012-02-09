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
	  timebase(1000), displayGain(1), parameterChanged(true), inWindow(false)

{
	//setSampleRate(25000.0);
	//numInputs = 16;

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

	int nSamples = (int) sampleRate*10.0f;
	int nInputs = getNumInputs();
	std::cout << "Setting inputs. Samples: " << nSamples << ", Inputs: " << nInputs << std::endl;

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

	glClearColor (0.2, 0.1, 0.9, 1.0);
	resized();

}


void LfpDisplayNode::renderOpenGL()
{

	//makeCurrentContextActive();
	
	glClear(GL_COLOR_BUFFER_BIT); // clear buffers to preset values

	//drawTicks();

	// updateScreenBuffer();

	// for (int i = 0; i < nChans; i++)
	// {
	// 	bool isSelected = false;

	// 	if (selectedChan == i)
	// 		isSelected = true;

	// 	if (checkBounds(i)) {
	// 		setViewport(i);
	// 		drawBorder(isSelected);
	// 		drawChannelInfo(i,isSelected);
	// 		//drawWaveform(i,isSelected);
	// 	}	
	// }
	drawScrollBars();

	//makeCurrentContextInactive();
}

int LfpDisplayNode::getTotalHeight() 
{
	return 800;
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
