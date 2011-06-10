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

class ProcessorGraph : public AudioProcessorGraph
{
public:
	ProcessorGraph(int numChannels);
	~ProcessorGraph();

	void* createNewProcessor(const String& description);

	int numSamplesInThisBuffer;

	int currentNodeId;

	const CriticalSection lock;

	RecordNode* getRecordNode();

private:	




};



#endif  // __PROCESSORGRAPH_H_124F8B50__
