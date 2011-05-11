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

class ProcessorGraph : public AudioProcessorGraph
{
public:
	ProcessorGraph();
	~ProcessorGraph();

	int numSamplesInThisBuffer;

	const CriticalSection lock;

private:	




};



#endif  // __PROCESSORGRAPH_H_124F8B50__
