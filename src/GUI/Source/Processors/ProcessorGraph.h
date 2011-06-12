/*
  ==============================================================================

    ProcessorGraph.h
    Created: 30 Apr 2011 8:36:35pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __PROCESSORGRAPH_H_124F8B50__
#define __PROCESSORGRAPH_H_124F8B50__

#include "../../JuceLibraryCode/JuceHeader.h"

class RecordNode;
class SourceNode;

class ProcessorGraph : public AudioProcessorGraph
{
public:
	ProcessorGraph(int numChannels);
	~ProcessorGraph();

	void* createNewProcessor(const String& description);
	void removeProcessor(int nodeId);

	bool enableSourceNode();
	bool disableSourceNode();

	int numSamplesInThisBuffer;

	int currentNodeId;
	int lastNodeId;

	const CriticalSection lock;

	RecordNode* getRecordNode();
	SourceNode* getSourceNode();

private:	

	int SOURCE_NODE_ID;
	const int RECORD_NODE_ID;
	const int DISPLAY_NODE_ID;
	const int OUTPUT_NODE_ID;
	const int RESAMPLING_NODE_ID;

	Array<int, CriticalSection> nodeArray; 

};



#endif  // __PROCESSORGRAPH_H_124F8B50__
