/*
  ==============================================================================

    SourceNode.cpp
    Created: 7 May 2011 5:07:14pm
    Author:  jsiegle

  ==============================================================================
*/


#include "SourceNode.h"
#include <stdio.h>

SourceNode::SourceNode(const String name_, int* nSamps, int nChans, const CriticalSection& lock_)
	: GenericProcessor(name_, nSamps, nChans, lock_),
	  transmitData(false), dataThread(0)
{

	// Source node type determines configuration info
	if (getName().equalsIgnoreCase("Intan Demo Board")) {

		setPlayConfigDetails(0, // numInputChannels
						 16, // numOutputChannels
						 44100.0, // expected sample rate
						 128); // expected number of samples per buf

	}

}

SourceNode::~SourceNode() {}

void SourceNode::setParameter (int parameterIndex, float newValue)
{
	//std::cout << "Got parameter change notification";
}

void SourceNode::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{

	if (transmitData) {
		std::cout << "Prepared to play." << std::endl;

		// Source node determines type of data thread
		if (getName().equalsIgnoreCase("Intan Demo Board")) {
			dataThread = new IntanThread();
			inputBuffer = dataThread->getBufferAddress();
		}

	} else {
		transmitData = true;
	}

}

//void SourceNode::createEditor() {
	
//}


void SourceNode::releaseResources() 
{	
	if (dataThread != 0) {
		delete dataThread;
		dataThread = 0;
	}
}

void SourceNode::processBlock (AudioSampleBuffer& outputBuffer, MidiBuffer& midiMessages)
{

	outputBuffer.clear();
	int numRead = inputBuffer->readAllFromBuffer(outputBuffer,outputBuffer.getNumSamples());
	// write the total number of samples
	setNumSamples(numRead);

}



