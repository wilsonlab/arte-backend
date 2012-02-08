/*
  ==============================================================================

    LfpDisplayNode.h
    Created: 8 Feb 2012 12:31:32pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __LFPDISPLAYNODE_H_D969A379__
#define __LFPDISPLAYNODE_H_D969A379__

#include "../../JuceLibraryCode/JuceHeader.h"
#include "Editors/LfpDisplayEditor.h"
#include "GenericProcessor.h"

class DataViewport;

class LfpDisplayNode : public GenericProcessor

{
public:

	LfpDisplayNode();
	~LfpDisplayNode();

	AudioProcessorEditor* createEditor();

	bool isSink() {return true;}

	void process(AudioSampleBuffer &buffer, MidiBuffer &midiMessages, int& nSamples);

private:

	DataViewport* dataViewport;
	AudioSampleBuffer* displayBuffer;
	MidiBuffer* eventBuffer;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LfpDisplayNode);

};




#endif  // __LFPDISPLAYNODE_H_D969A379__
