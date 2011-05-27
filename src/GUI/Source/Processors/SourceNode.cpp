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
	  name (name_), lock(lock_), transmitData(false), dataThread(0)
{
	setPlayConfigDetails(0, // numInputChannels
						 16, // numOutputChannels
						 44100.0, // expected sample rate
						 128); // expected number of samples per buf

	//suspendProcessing(true);

}

SourceNode::~SourceNode()
{

}



void SourceNode::setParameter (int parameterIndex, float newValue)
{

	//std::cout << "Got parameter change notification";

	//suspendProcessing(!isSuspended());

}

void SourceNode::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{

	if (transmitData) {
	//	playRequest++;
		std::cout << "Prepared to play." << std::endl;

		dataThread = new DataThread();
		//for (int chan = 0; )
		inputBuffer = dataThread->getBufferAddress();

	} else {
		transmitData = true;
	}

}


void SourceNode::releaseResources() 
{	

	if (dataThread != 0) {
		delete dataThread;
		dataThread = 0;
	}

}

void SourceNode::start() {
	
	//transmitData = false;
	//std::cout << transmitData << std::endl;
	//std::cout << "starting." << std::endl;
	//std::cout << dataThread << std::endl;
	//if (dataThread != 0) {
	//	dataThread->start();
	//}

	//suspendProcessing (false);

	//std::count << isSuspended();

}

void SourceNode::stop() {
	
	//transmitData = true;
	//std::cout << transmitData << std::endl;
	//std::cout << "stopping." << std::endl;
	//if (dataThread != 0) {
	//	dataThread->stop();
	//}

	//suspendProcessing (true);

}

void SourceNode::processBlock (AudioSampleBuffer& outputBuffer, MidiBuffer& midiMessages)
{

	//std::cout << "SourceNode processing." << std::endl;
		
	// copy from data thread buffer into outputBuffer (assumes 16 channels)

		outputBuffer.clear();

		// number of samples in buffer fluctuates a lot,
		// not sure if this is the fault of the FPGA or the callback timing
		// we may want to put this line after a 'while' loop that waits
		// for the appropriate number of samples
	//	while (inputBuffer->getNumSamples() < roundToInt(outputBuffer.getNumSamples()*25000/44100))
//			{ // wait
//			} 

		//std::cout << transmitData << std::endl;
	
		//if (!isSuspended()) {
			//std::cout << "transmitting!" << std::endl;
			int numRead = inputBuffer->readAllFromBuffer(outputBuffer,outputBuffer.getNumSamples());
	
	// write the total number of samples
			lock.enter();
			*numSamplesInThisBuffer = numRead;
			lock.exit();
		//} else {
		//	lock.enter();
		//	*numSamplesInThisBuffer = outputBuffer.getNumSamples();
		//	lock.exit();
		//}



		//std::cout << "Source Node sample count: " << outputBuffer.getNumSamples() << std::endl;

		//std::cout << "Source node samples: " << numRead << std::endl;

		//std::cout << numRead << std::endl;

}



