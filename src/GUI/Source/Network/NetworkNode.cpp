/*
  ==============================================================================

    NetworkNode.cpp
    Created: 27 May 2011 1:29:49pm
    Author:  jsiegle

  ==============================================================================
*/

/*
  ==============================================================================

    SourceNode.cpp
    Created: 7 May 2011 5:07:14pm
    Author:  jsiegle

  ==============================================================================
*/


#include "NetworkNode.h"
#include <stdio.h>

NetworkNode::NetworkNode(const String name_, int* nSamps, const CriticalSection& lock_)
	: numSamplesInThisBuffer(nSamps),
	  name (name_), lock(lock_), transmitData(false), networkThread(0)
{
	setPlayConfigDetails(0, // numInputChannels
						 8, // numOutputChannels
						 44100.0, // expected sample rate
						 128); // expected number of samples per buf

	//suspendProcessing(true);

}

NetworkNode::~NetworkNode()
{

}



void NetworkNode::setParameter (int parameterIndex, float newValue)
{

	//std::cout << "Got parameter change notification";

	//suspendProcessing(!isSuspended());

}

void NetworkNode::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{

	if (transmitData) {
	//	playRequest++;
		std::cout << "Prepared to play." << std::endl;

		networkThread = new NetworkThread();
		inputBuffer = networkThread->getBufferAddress();

	} else {
		transmitData = true;
	}

}


void NetworkNode::releaseResources() 
{	

	if (networkThread != 0) {
		delete networkThread;
		networkThread = 0;
	}

}


void NetworkNode::processBlock (AudioSampleBuffer& outputBuffer, MidiBuffer& midiMessages)
{

	//std::cout << "SourceNode processing." << std::endl;

		outputBuffer.clear();

			int numRead = inputBuffer->readAllFromBuffer(outputBuffer,outputBuffer.getNumSamples());
	
	// write the total number of samples
			lock.enter();
			*numSamplesInThisBuffer = numRead;
			lock.exit();

}



