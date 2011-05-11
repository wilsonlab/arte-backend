/*
  ==============================================================================

    SourceNode.cpp
    Created: 7 May 2011 5:07:14pm
    Author:  jsiegle

  ==============================================================================
*/


#include "SourceNode.h"
#include <stdio.h>

SourceNode::SourceNode(const String name_, int* nSamps, const CriticalSection& lock_)
	: numSamplesInThisBuffer(nSamps),
	  name (name_), lock(lock_)
{
	setPlayConfigDetails(0, // numInputChannels
						 16, // numOutputChannels
						 44100.0, // expected sample rate
						 128); // expected number of samples per buffer
}

SourceNode::~SourceNode()
{

}



void SourceNode::setParameter (int parameterIndex, float newValue)
{

}

void SourceNode::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{

	dataThread = new DataThread();
	//for (int chan = 0; )
	inputBuffer = dataThread->getBufferAddress();

}


void SourceNode::releaseResources() 
{	

	delete dataThread;
	dataThread = 0;

}

void SourceNode::processBlock (AudioSampleBuffer& outputBuffer, MidiBuffer& midiMessages)
{

		
	// copy from data thread buffer into outputBuffer (assumes 16 channels)

		outputBuffer.clear();

		// number of samples in buffer fluctuates a lot,
		// not sure if this is the fault of the FPGA or the callback timing
		// we may want to put this line after a 'while' loop that waits
		// for the appropriate number of samples
		while (dataBuffer->getNumSamples() < roundToInt(outputBuffer.getNumSamples()*25000/44100)
			;
		
		int numRead = inputBuffer->readAllFromBuffer(outputBuffer,outputBuffer.getNumSamples());
	
	// write the total number of samples
		lock.enter();
		*numSamplesInThisBuffer = numRead;
		lock.exit();

		//std::cout << "Source Node sample count: " << numRead << std::endl;

		//std::cout << "Source node samples: " << numRead << std::endl;

		//std::cout << numRead << std::endl;

}



