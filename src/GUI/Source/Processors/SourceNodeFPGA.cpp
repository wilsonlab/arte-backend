/*
  ==============================================================================

    SourceNodeFPGA.cpp
    Created: 10 May 2011 6:48:56pm
    Author:  jsiegle

  ==============================================================================
*/



#include "SourceNodeFPGA.h"
#include <stdio.h>

SourceNodeFPGA::SourceNodeFPGA(const String name_, int* nSamps, const CriticalSection& lock_)
	: numSamplesInThisBuffer(nSamps),
	  name (name_), lock(lock_)
{
	setPlayConfigDetails(0, // numInputChannels
						 16, // numOutputChannels
						 44100.0, // expected sample rate
						 128); // expected number of samples per buffer
}

SourceNodeFPGA::~SourceNodeFPGA()
{

}



void SourceNodeFPGA::setParameter (int parameterIndex, float newValue)
{

}

void SourceNodeFPGA::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{

	dataThread = new DataThread();
	//for (int chan = 0; )
	inputBuffer = dataThread->getBufferAddress();

}


void SourceNodeFPGA::releaseResources() 
{	

	delete dataThread;
	dataThread = 0;

}

void SourceNodeFPGA::processBlock (AudioSampleBuffer& outputBuffer, MidiBuffer& midiMessages)
{

		
	// copy from data thread buffer into outputBuffer (assumes 16 channels)

		outputBuffer.clear();


		// the number of samples in buffer fluctuates a lot,
		// not sure if this is the fault of the FPGA or the callback timing
		// we may want to put this line after a 'while' loop that waits
		// for the appropriate number of samples, e.g.:
		//while (dataBuffer->getNumSamples() < outputBuffer.getNumSamples()*25000/44100)
		//	;
		

		int numRead = inputBuffer->readAllFromBuffer(outputBuffer,outputBuffer.getNumSamples());
	
	// write the total number of samples
		lock.enter();
		*numSamplesInThisBuffer = numRead;
		lock.exit();

		//std::cout << "Source Node sample count: " << numRead << std::endl;

		//std::cout << "Source node samples: " << numRead << std::endl;

		//std::cout << numRead << std::endl;

}



