/*
  ==============================================================================

    SpikeDetector.h
    Created: 14 Aug 2011 3:36:00pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __SPIKEDETECTOR_H_3F920F95__
#define __SPIKEDETECTOR_H_3F920F95__

#include "../../JuceLibraryCode/JuceHeader.h"
#include "GenericProcessor.h"
#include "Editors/SpikeDetectorEditor.h"

class SpikeDetectorEditor;
class FilterViewport;

class SpikeDetector : public GenericProcessor

{
public:
	
	SpikeDetector(const String name, int* nSamples, int nChans, const CriticalSection& lock, int nodeId);
	~SpikeDetector();
	
	void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock);
	void releaseResources();
	void processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages);
	void setParameter (int parameterIndex, float newValue);

	MidiBuffer* getEventBuffer() {return spikeBuffer;}

	AudioProcessorEditor* createEditor();

	bool hasEditor() const {return true;}
	
private:
	double sampleRate, threshold;
	double prePeakMs, postPeakMs;
	int prePeakSamples, postPeakSamples;
	int accumulator;

	MidiBuffer* spikeBuffer;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpikeDetector);

};



#endif  // __SPIKEDETECTOR_H_3F920F95__
