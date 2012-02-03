/*
  ==============================================================================

    SourceNode.cpp
    Created: 7 May 2011 5:07:14pm
    Author:  jsiegle

  ==============================================================================
*/


#include "SourceNode.h"
#include "Editors/SourceNodeEditor.h"
#include <stdio.h>

SourceNode::SourceNode(const String& name_)
	: GenericProcessor(name_),
	  dataThread(0)
{
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

	setPlayConfigDetails(getNumInputs(), getNumOutputs(), 44100.0, 128);

}

SourceNode::~SourceNode() {}

// void SourceNode::setName(const String name_)
// {
// 	name = name_;

// 	// Source node type determines configuration info


// }

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


AudioProcessorEditor* SourceNode::createEditor()
{
	SourceNodeEditor* ed = new SourceNodeEditor(this, viewport);
	setEditor(ed);
	
	std::cout << "Creating editor." << std::endl;
	//filterEditor = new FilterEditor(this);
	return ed;

	//return 0;
}

// void SourceNode::setSourceNode(GenericProcessor* sn) 
// {
// 	sourceNode = 0;
// }

// void SourceNode::setDestNode(GenericProcessor* dn)
// {
// 	destNode = dn;
// 	if (dn != 0)
// 		dn->setSourceNode(this);
// }

//void SourceNode::createEditor() {
	
//}

void SourceNode::enable() {
	
	std::cout << "Source node received enable signal" << std::endl;

	if (getName().equalsIgnoreCase("Intan Demo Board")) {
		dataThread = new IntanThread();
		inputBuffer = dataThread->getBufferAddress();
	} else if (getName().equalsIgnoreCase("Custom FPGA")) {
		dataThread = new FPGAThread();
		inputBuffer = dataThread->getBufferAddress();
	} else if (getName().equalsIgnoreCase("File Reader")) {
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
	//std::cout << outputBuffer.getNumChannels() << " " << outputBuffer.getNumSamples() << std::endl;

	
	 outputBuffer.clear();
	 int numRead = inputBuffer->readAllFromBuffer(outputBuffer,outputBuffer.getNumSamples());
	// //setNumSamples(numRead); // write the total number of samples
	 setNumSamples(midiMessages, numRead);
	//std::cout << numRead << std::endl;
}



