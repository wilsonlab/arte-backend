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
#include "AudioNode.h"
#include "ResamplingNode.h"
#include "SignalGenerator.h"
#include "RecordNode.h"
#include "EventNode.h"
#include "DisplayNode.h"
#include <stdio.h>

ProcessorGraph::ProcessorGraph(int numChannels) : currentNodeId(100), lastNodeId(1), 
	SOURCE_NODE_ID(0), 
	RECORD_NODE_ID(199), 
	AUDIO_NODE_ID(10), 
	OUTPUT_NODE_ID(201), 
	RESAMPLING_NODE_ID(202),
	numSamplesInThisBuffer(1024)
	
	{

	setPlayConfigDetails(0,2,44100.0, 128);

	createDefaultNodes();

}

ProcessorGraph::~ProcessorGraph() { }


void ProcessorGraph::createDefaultNodes()
{

	// add output node
	AudioProcessorGraph::AudioGraphIOProcessor* on = 
		new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);

	// add record node
	RecordNode* recn = new RecordNode(T("Record Node"), &numSamplesInThisBuffer, 1024, lock, RECORD_NODE_ID);

	// add audio node
	AudioNode* an = new AudioNode(T("Audio Node"), &numSamplesInThisBuffer, 1024, lock, AUDIO_NODE_ID);

	// add resampling node
	ResamplingNode* rn = new ResamplingNode(T("Resampling Node"), &numSamplesInThisBuffer, 2, lock, RESAMPLING_NODE_ID, true);

	addNode(on,OUTPUT_NODE_ID);
	addNode(recn,RECORD_NODE_ID);
	addNode(an, AUDIO_NODE_ID);
	addNode(rn, RESAMPLING_NODE_ID);

	// connect audio network
	for (int n = 0; n < 2; n++) {
		
		addConnection(AUDIO_NODE_ID, n,
		              RESAMPLING_NODE_ID, n);
		
		addConnection(RESAMPLING_NODE_ID, n,
		              OUTPUT_NODE_ID, n);

	}

	std::cout << "Default nodes created." << std::endl;
	
}


void* ProcessorGraph::createNewProcessor(const String& description, FilterViewport* vp) {

	int splitPoint = description.indexOf("/");
	String processorType = description.substring(0,splitPoint);
	String subProcessorType = description.substring(splitPoint+1);

	std::cout << processorType << "::" << subProcessorType << std::endl;

	GenericProcessor* processor = 0;

	int id = currentNodeId;

	if (processorType.equalsIgnoreCase("Data Sources")) {

		if (SOURCE_NODE_ID == 0) {
			std::cout << "Creating a new data source." << std::endl;
			processor = new SourceNode(subProcessorType, &numSamplesInThisBuffer, 16, lock, id);
			SOURCE_NODE_ID = id;
			if (nodeArray.size() > 1)
				nodeArray.set(0, id);
			else 
				nodeArray.add(id);
		}


	} else if (processorType.equalsIgnoreCase("Filters")) {
		if (subProcessorType.equalsIgnoreCase("Bandpass Filter")) {
			std::cout << "Creating a new filter." << std::endl;
			processor = new FilterNode(subProcessorType, &numSamplesInThisBuffer, getSourceNode()->getNumOutputs(), lock, id);

		} else if (subProcessorType.equalsIgnoreCase("Resampler")) {
			std::cout << "Creating a new resampler." << std::endl;
			processor = new ResamplingNode(subProcessorType, &numSamplesInThisBuffer, getSourceNode()->getNumOutputs(), lock, id, false);
		} 

	} else if (processorType.equalsIgnoreCase("Utilities")) {
		
		if (subProcessorType.equalsIgnoreCase("Event Node")) {
			
			if (SOURCE_NODE_ID == 0) {
				SOURCE_NODE_ID = id;
				std::cout << "Creating a new event source." << std::endl;
				processor = new EventNode(subProcessorType, &numSamplesInThisBuffer, 2, lock, SOURCE_NODE_ID, true);
			} else {
				std::cout << "Creating a new event receiver." << std::endl;
				processor = new EventNode(subProcessorType, &numSamplesInThisBuffer, 2, lock, currentNodeId, false);
				
				std::cout << midiChannelIndex << " is MIDI index." << std::endl;

			}

		}


	} else if (processorType.equalsIgnoreCase("Visualizers")) {
		
		if (subProcessorType.equalsIgnoreCase("Stream Viewer")) {
			
			std::cout << "Creating a display node." << std::endl;
			processor = new DisplayNode(subProcessorType, &numSamplesInThisBuffer, 16, lock, currentNodeId);
		}
	
	
	} else {

		processor = new GenericProcessor(subProcessorType, &numSamplesInThisBuffer, 16, lock, id);

	}

	if (processor != 0) {

		std::cout << "Adding node to graph." << std::endl;
		addNode(processor,id);

		if (false) {//id != SOURCE_NODE_ID) {
		
			std::cout << "Connecting to source node." << std::endl;

			for (int chan = 0; chan < processor->getNumInputs(); chan++) {
				addConnection(nodeArray.getLast(), // sourceNodeID
				  	chan, // sourceNodeChannelIndex
				   	id, // destNodeID
				  	chan); // destNodeChannelIndex
			}

			std::cout << "Connecting to output nodes." << std::endl;

			for (int chan = 0; chan < processor->getNumOutputs(); chan++) {

				addConnection(id, // sourceNodeID
				  	chan, // sourceNodeChannelIndex
				   	AUDIO_NODE_ID, // destNodeID
				  	chan); // destNodeChannelIndex

				addConnection(id, // sourceNodeID
				  	chan, // sourceNodeChannelIndex
				   	RECORD_NODE_ID, // destNodeID
				  	chan); // destNodeChannelIndex
			}

		}

			addConnection(SOURCE_NODE_ID, // sourceNodeID
				  	midiChannelIndex, // sourceNodeChannelIndex
				   	currentNodeId, // destNodeID
				  	midiChannelIndex); // destNodeChannelIndex

		nodeArray.add(id);
		currentNodeId++;

		processor->setViewport(vp);
		return processor->createEditor();

	} else {
		return 0;
	}

}

void ProcessorGraph::removeProcessor(int nodeId) {
	std::cout << "Removing processor with ID " << nodeId << std::endl;

	if (nodeId == SOURCE_NODE_ID) {
		SOURCE_NODE_ID = 0;
	}

	removeNode(nodeId);
	nodeArray.remove(nodeArray.indexOf(nodeId));
}

bool ProcessorGraph::enableSourceNode() {
	//std::cout << "Enabling source node..." << std::endl;
	SourceNode* sn = getSourceNode();

	if (sn != 0) {
		sn->enable();
		return true;
	} else {
		return false;
	}
}

bool ProcessorGraph::disableSourceNode() {

	//std::cout << "Disabling source node..." << std::endl;
	SourceNode* sn = getSourceNode();

	if (sn != 0) {
		sn->disable();
		return true;
	} else {
		return false;
	}
}


RecordNode* ProcessorGraph::getRecordNode() {
	
	Node* node = getNodeForId(RECORD_NODE_ID);
	return (RecordNode*) node->getProcessor();

}

SourceNode* ProcessorGraph::getSourceNode() {
	
	if (nodeArray.size() > 0 && nodeArray[0] == SOURCE_NODE_ID) {
		Node* node = getNodeForId(SOURCE_NODE_ID);
		return (SourceNode*) node->getProcessor();
	} else {
		return 0;
	}

}