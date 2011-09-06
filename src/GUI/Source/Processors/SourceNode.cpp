/*
  ==============================================================================

    SourceNode.cpp
    Created: 7 May 2011 5:07:14pm
    Author:  jsiegle

  ==============================================================================
*/


#include "SourceNode.h"
#include <stdio.h>

SourceNode::SourceNode(const String name_, int* nSamps, int nChans, const CriticalSection& lock_, int id)
	: GenericProcessor(name_, nSamps, nChans, lock_, id),
	  dataThread(0)
{

	// Source node type determines configuration info
	if (getName().equalsIgnoreCase("Intan Demo Board")) {
		setNumOutputs(16);
		setNumInputs(0);
	} else if (getName().equalsIgnoreCase("Custom FPGA")) {
		setNumOutputs(32);
		setNumInputs(0);
	} else if (getName().equalsIgnoreCase("File Reader")) {
		setNumOutputs(16);
		setNumInputs(0);
	}

}

SourceNode::~SourceNode() {}

void SourceNode::setParameter (int parameterIndex, float newValue)
{
	//std::cout << "Got parameter change notification";
}

void SourceNode::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{
	//
	// We take care of thread creation and destruction in separate enable/disable function
	// 
	// prepareToPlay is called whenever the graph is edited, not only when callbacks are
	// 	about to begin
	//
}

void SourceNode::releaseResources() {}

//void SourceNode::createEditor() {
	
//}

void SourceNode::enable() {
	
	std::cout << "Source node received enable signal" << std::endl;

	if (getName().equalsIgnoreCase("Data Sources/Intan Demo Board")) {
		dataThread = new IntanThread();
		inputBuffer = dataThread->getBufferAddress();
	} else if (getName().equalsIgnoreCase("Data Sources/Custom FPGA")) {
		dataThread = new FPGAThread();
		inputBuffer = dataThread->getBufferAddress();
	} else if (getName().equalsIgnoreCase("Data Sources/File Reader")) {
		dataThread = new FileReaderThread();
		inputBuffer = dataThread->getBufferAddress();
	}

}

void SourceNode::disable() {
	
	std::cout << "Source node received disable signal" << std::endl;

	if (dataThread != 0) {
		delete dataThread;
		dataThread = 0;
	}
}


void SourceNode::processBlock (AudioSampleBuffer& outputBuffer, MidiBuffer& midiMessages)
{

	//std::cout << "Source node processing." << std::endl;

	outputBuffer.clear();
	int numRead = inputBuffer->readAllFromBuffer(outputBuffer,outputBuffer.getNumSamples());
	setNumSamples(numRead); // write the total number of samples
}



