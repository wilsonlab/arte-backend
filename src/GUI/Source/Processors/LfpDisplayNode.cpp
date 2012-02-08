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
	: GenericProcessor("LFP Viewer")

{
	displayBuffer = new AudioSampleBuffer(16,2048);
	eventBuffer = new MidiBuffer();
}

LfpDisplayNode::~LfpDisplayNode()
{
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

void LfpDisplayNode::process(AudioSampleBuffer &buffer, MidiBuffer &midiMessages, int& nSamples)
{
	

}

