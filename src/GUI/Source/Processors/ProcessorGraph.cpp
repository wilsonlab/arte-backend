/*
  ==============================================================================

    ProcessorGraph.cpp
    Created: 30 Apr 2011 8:36:35pm
    Author:  jsiegle

  ==============================================================================
*/


#include "ProcessorGraph.h"
#include "SourceNode.h"
#include "FilterNode.h"
#include "GenericProcessor.h"
#include "DisplayNode.h"
#include "ResamplingNode.h"
#include "SignalGenerator.h"
//#include "../Network/NetworkNode.h"
#include "RecordNode.h"
#include <stdio.h>

ProcessorGraph::ProcessorGraph(int numChannels) : currentNodeId(100) {

	//std::cout << "Processor graph created." << std::endl;

	setPlayConfigDetails(0,2,44100.0, 128);

	numSamplesInThisBuffer = 1024;

	//SignalGenerator* sg = new SignalGenerator();
	//NetworkNode* sn = new NetworkNode(T("Processor 1"), &numSamplesInThisBuffer, lock);
	SourceNode* sn = new SourceNode("Intan Demo Board", &numSamplesInThisBuffer, 16, lock);
	FilterNode* fn = new FilterNode(T("Filter Node"), &numSamplesInThisBuffer, lock);
	DisplayNode* dn = new DisplayNode(T("Display Node"), &numSamplesInThisBuffer, lock);
	ResamplingNode* rn = new ResamplingNode(T("Resampling Node"), &numSamplesInThisBuffer, lock, true);
	//GenericProcessor* gp2 = new GenericProcessor(T("Processor 2"), &numSamplesInThisBuffer);

	RecordNode* recn = new RecordNode(T("Record Node"), &numSamplesInThisBuffer, lock);
	//GenericProcessor* gp3 = new GenericProcessor(T("Output Node"));

	// add output node
	AudioProcessorGraph::AudioGraphIOProcessor* on = 
		new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);

	//addNode(sg,98);
	addNode(sn,98);
	addNode(fn,3);
	addNode(dn,10);
	addNode(rn,2);
	addNode(recn,15);
	//addNode(gp2,2);
	addNode(on,99);

	for (int chan = 0; chan < numChannels; chan++) {

		// connect source node to filter node

	addConnection(98, // sourceNodeID
				  chan, // sourceNodeChannelIndex
				  3, // destNodeID
				  chan); // destNodeChannelIndex


	    // connect filter node to resampling node

		 addConnection(3, // sourceNodeID
		 		  chan, // sourceNodeChannelIndex
		 		  2, // destNodeID
		 		  chan); // destNodeChannelIndex


	   // connect filter node to display node

	  	addConnection(3, // sourceNodeID
				  chan, // sourceNodeChannelIndex
				  10, // destNodeID
				  chan); // destNodeChannelIndex

    }

    // connect to record node
    addConnection(3,0,15,0);
    addConnection(3,1,15,1);
    addConnection(3,2,15,2);

    //  connect resampling node to output node
    addConnection(2, // sourceNodeID
				  0, // sourceNodeChannelIndex
				  99, // destNodeID
				  0); // destNodeChannelIndex

	addConnection(2, // sourceNodeID
				  1, // sourceNodeChannelIndex
				  99, // destNodeID
				  1); // destNodeChannelIndex

    std::cout << "Processor graph created." << std::endl;
	
}

ProcessorGraph::~ProcessorGraph() {
	


}

void* ProcessorGraph::createNewProcessor(const String& description) {
	
	std::cout << "Creating new processor with description: " << description << std::endl;
	GenericProcessor* gp = new GenericProcessor(T("Display Node"), &numSamplesInThisBuffer, 16, lock);
	addNode(gp,currentNodeId++);
	return gp->createEditor();
}