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
#include <stdio.h>

ProcessorGraph::ProcessorGraph() {

	//std::cout << "Processor graph created." << std::endl;

	setPlayConfigDetails(0,2,44100.0, 128);

	numSamplesInThisBuffer = 1024;

	//SignalGenerator* sg = new SignalGenerator();
	SourceNode* sn = new SourceNode(T("Processor 1"), &numSamplesInThisBuffer, lock);
	FilterNode* fn = new FilterNode(T("Filter Node"), &numSamplesInThisBuffer, lock);
	DisplayNode* dn = new DisplayNode(T("Display Node"), &numSamplesInThisBuffer, lock);
	ResamplingNode* rn = new ResamplingNode(T("Resampling Node"), &numSamplesInThisBuffer, lock, true);
	//GenericProcessor* gp2 = new GenericProcessor(T("Processor 2"), &numSamplesInThisBuffer);

	//GenericProcessor* gp3 = new GenericProcessor(T("Output Node"));

	// add output node
	AudioProcessorGraph::AudioGraphIOProcessor* on = 
		new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);

	//addNode(sg,98);
	addNode(sn,98);
	addNode(fn,3);
	addNode(dn,10);
	addNode(rn,2);
	//addNode(gp2,2);
	addNode(on,99);

	for (int chan = 0; chan < 16; chan++) {

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

    //  connect resampling node to output node
    addConnection(2, // sourceNodeID
				  1, // sourceNodeChannelIndex
				  99, // destNodeID
				  0); // destNodeChannelIndex

	addConnection(2, // sourceNodeID
				  1, // sourceNodeChannelIndex
				  99, // destNodeID
				  1); // destNodeChannelIndex

    std::cout << "Processor graph created." << std::endl;
	
	/*SourceNode* sn = new SourceNode();
	FilterNode* fn = new FilterNode();
	recordNode = new RecordNode();

	asb = new AudioSampleBuffer(2,1000);
	//msg = new Message();
	//asb.setSize(1,735);

	DisplayNode* dn = new DisplayNode(*asb);
	AudioProcessorGraph::AudioGraphIOProcessor* on = 
		new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);

	graph.addNode(sn,5);
	graph.addNode(fn,7);
	graph.addNode(dn,10);
	graph.addNode(on,13);
	graph.addNode(recordNode,15);
	
	for (int chan = 0; chan < 1; chan++) {
	graph.addConnection(5, // sourceNodeID
				 		chan, // sourceNodeChannelIndex
				 		7, // destNodeID
				 		chan); // destNodeChannelIndex
	
	graph.addConnection(7, // sourceNodeID
				 		chan, // sourceNodeChannelIndex
				 		10, // destNodeID
				 		chan); // destNodeChannelIndex

		graph.addConnection(7, // sourceNodeID
				 		chan, // sourceNodeChannelIndex
				 		15, // destNodeID
				 		chan); // destNodeChannelIndex

	graph.addConnection(7, // sourceNodeID
				 		chan, // sourceNodeChannelIndex
				 		13, // destNodeID
				 		chan); // destNodeChannelIndex
	}*/


}

ProcessorGraph::~ProcessorGraph() {
	


}