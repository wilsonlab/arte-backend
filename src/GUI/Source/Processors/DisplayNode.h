/*
  ==============================================================================

    DisplayNode.h
    Created: 7 May 2011 5:07:43pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __DISPLAYNODE_H_C7B28CA4__
#define __DISPLAYNODE_H_C7B28CA4__


#include "../../JuceLibraryCode/JuceHeader.h"
#include "Editors/StreamViewer.h"
#include "ResamplingNode.h"

//class StreamViewer;

class DisplayNode : public ResamplingNode

{
public:
	// real member functions:
	DisplayNode(const String name, int* nSamples, int nChans, const CriticalSection& lock, int nodeId);
	~DisplayNode();

	AudioProcessorEditor* createEditor();
	
private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DisplayNode);

};



#endif  // __DISPLAYNODE_H_C7B28CA4__
