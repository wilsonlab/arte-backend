/*
  ==============================================================================

    FilterNode.h
    Created: 7 May 2011 5:07:28pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __FILTERNODE_H_CED428E__
#define __FILTERNODE_H_CED428E__

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Dsp/Dsp.h"
#include "GenericProcessor.h"
#include "Editors/FilterEditor.h"

class FilterEditor;
class FilterViewport;

class FilterNode : public GenericProcessor

{
public:
	
	FilterNode(const String name, int* nSamples, int nChans, const CriticalSection& lock, int nodeId);
	~FilterNode();
	
	void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock);
	void releaseResources();
	void processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages);
	void setParameter (int parameterIndex, float newValue);

	AudioProcessorEditor* createEditor();

	bool hasEditor() const {return true;}
	
private:
	double sampleRate, centerFrequency, bandWidth;
	Dsp::Filter* filter;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterNode);

};





#endif  // __FILTERNODE_H_CED428E__
