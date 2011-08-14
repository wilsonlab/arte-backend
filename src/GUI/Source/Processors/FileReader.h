/*
  ==============================================================================

    FileReader.h
    Created: 13 Aug 2011 7:18:22pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __FILEREADER_H_605BF4A__
#define __FILEREADER_H_605BF4A__


#include "../../JuceLibraryCode/JuceHeader.h"
#include "GenericProcessor.h"

//class FileReaderEditor;

class FileReader : public GenericProcessor

{
public:
	
	// real member functions:
	FileReader(const String name, int* nSamples, int nChans, const CriticalSection& lock, int nodeId);
	~FileReader();
	
	void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock);
	void releaseResources();
	void processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

	void setParameter (int parameterIndex, float newValue);

	//AudioProcessorEditor* createEditor();
	bool hasEditor() const {return true;}

	void enable();
	void disable();

	bool isSource() {return true;}

private:

	float sampleRate;
	int numChannels;
	int samplesPerBlock;

	FileInputStream* input;

	//SourceNodeEditor* sourceEditor;
	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileReader);

};




#endif  // __FILEREADER_H_605BF4A__
