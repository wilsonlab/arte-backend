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

class GenericProcessor;
class RecordNode;
class SourceNode;
class FilterViewport;
class UIComponent;

class ProcessorGraph : public AudioProcessorGraph,
					   public ActionBroadcaster
{
public:
	ProcessorGraph(int numChannels);
	~ProcessorGraph();

	void* createNewProcessor(String& description, 
							 FilterViewport* viewport,
							 GenericProcessor* source,
							 GenericProcessor* dest);

	GenericProcessor* createProcessorFromDescription(String& description);

	void removeProcessor(GenericProcessor* processor);

	bool enableSourceNode();
	bool disableSourceNode();

	int numSamplesInThisBuffer;

	int currentNodeId;
	int lastNodeId;

	const CriticalSection lock;

	RecordNode* getRecordNode();
	SourceNode* getSourceNode();

	void setUIComponent(UIComponent* ui) {UI = ui;}

	const String saveState(const File& file);
	const String loadState(const File& file);

	XmlElement* createNodeXml(GenericProcessor*);

private:	

	int SOURCE_NODE_ID;
	const int RECORD_NODE_ID;
	const int AUDIO_NODE_ID;
	const int OUTPUT_NODE_ID;
	const int RESAMPLING_NODE_ID;

	//Array<int, CriticalSection> nodeArray; 

	void createDefaultNodes();

	UIComponent* UI;



};



#endif  // __PROCESSORGRAPH_H_124F8B50__
