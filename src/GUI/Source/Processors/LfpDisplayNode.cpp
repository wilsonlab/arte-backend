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
	  timebase(1000), displayGain(1), parameterChanged(true)

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
