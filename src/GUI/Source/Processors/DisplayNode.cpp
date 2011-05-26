/*
  ==============================================================================

    DisplayNode.cpp
    Created: 7 May 2011 5:07:43pm
    Author:  jsiegle

  ==============================================================================
*/


#include "DisplayNode.h"
#include <stdio.h>

DisplayNode::DisplayNode(const String name_, int* nSamps, const CriticalSection& lock_)
	: name (name_), numSamplesInThisBuffer(nSamps), lock(lock_),
	   sampleRate (25000.0),
	  refreshRate (60),
	  displayTimebase(1.0)
	  
{

	setPlayConfigDetails(16,0,44100.0,128);

	displayBuffer = new AudioSampleBuffer(16,1000);

	displayBuffer->clear();
	maxBufferSize = sampleRate / refreshRate;

	accumulator = 0;
	pixelValue = 0;
	bufferIndex = 0;

}

DisplayNode::~DisplayNode()
{
	streamViewer = 0;
	deleteAndZero(displayBuffer);
}

StreamViewer* DisplayNode::createDisplay( )
{
	//filterEditor = new FilterEditor(this);
	
	std::cout << "Creating editor." << std::endl;
	streamViewer = new StreamViewer(displayBuffer);
	
	//displayPort->selectBuffer(displayBuffer);
	
	return streamViewer;
	
}

/*DisplayPort* DisplayNode::createEditor( )
{
	//filterEditor = new FilterEditor(this);
	
	std::cout << "Creating editor." << std::endl;
	displayPort = new DisplayPort(this, displayBuffer);
	
	//displayPort->selectBuffer(displayBuffer);

	return displayPort;

}*/

//AudioProcessorEditor* FilterNode::createEditor(AudioProcessorEditor* const editor)
//{
	
//	return editor;
//}
void DisplayNode::setParameter (int parameterIndex, float newValue)
{
	//std::cout << "Message received." << std::endl;
	//centerFrequency = newValue;
	
	// buffer has been cleared
	
	//displayWidth = newValue;
	samplesPerPixel = displayTimebase / displayBuffer->getNumSamples() * sampleRate;
	bufferIndex = 0;

}


void DisplayNode::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{
	samplesPerPixel = displayTimebase / displayBuffer->getNumSamples() * sampleRate;

	std::cout << "Samples per pixel: " << samplesPerPixel << std::endl;
}

void DisplayNode::releaseResources() 
{	
}

void DisplayNode::processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{

	 //std::cout << buffer.getNumSamples() << std::endl;
	// std::cout << "Audio Callback" << std::endl;
	
	//std::cout << buffer.getNumSamples() << std::endl;

	//	std::cout << "Display Node sample count: " << buffer.getNumSamples() << std::endl;


	lock.enter();
	int nSamps = *numSamplesInThisBuffer;
	lock.exit();

	//std::cout << "Display Node sample count: " << nSamps << std::endl;

	for (int n = 0; n < nSamps; n++ )
	{
		if (accumulator < samplesPerPixel)
		{
			//pixelValue = 10.0;
			
			//pixelValue = pixelValue + *buffer.getSampleData(0,n);
			
			++accumulator;

		} else {
			//pixelValue = pixelValue / samplesPerPixel;
			
			for (int chan = 0; chan < buffer.getNumChannels(); chan++) {

				//pixelValue = *;
				displayBuffer->copyFrom(chan,bufferIndex,
										buffer.getSampleData(chan,n),1,0.1f);

			}
			//std::cout << "added " << pixelValue << std::endl;
			accumulator = 0;
			//pixelValue = 0;
			
			bufferIndex = (++bufferIndex) % displayBuffer->getNumSamples();

			//std::cout << bufferIndex << std::endl;
			

			//if (bufferIndex == 0) {
				//displayBuffer.clear();
			//}

			//if (bufferIndex < displayWidth)
		//		++bufferIndex;
		//	else
		//		bufferIndex = 0;
		}
	}


   // std::cout << "Filter processing complete." << std::endl;

}
