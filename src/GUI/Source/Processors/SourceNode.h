/*
  ==============================================================================

    SourceNode.h
    Created: 7 May 2011 5:07:14pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __SOURCENODE_H_DCE798F1__
#define __SOURCENODE_H_DCE798F1__

#include "../../JuceLibraryCode/JuceHeader.h"
#include <ftdi.h>
#include <stdio.h>
#include "DataThreads/DataBuffer.h"
#include "DataThreads/IntanThread.h"
#include "GenericProcessor.h"

class SourceNode : public GenericProcessor

{
public:
	
	// real member functions:
	SourceNode(const String name, int* nSamples, int nChans, const CriticalSection& lock, int nodeId);
	~SourceNode();
	
	void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock);
	void releaseResources();
	void processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

	void setParameter (int parameterIndex, float newValue);

	//AudioProcessorEditor* createEditor();
	bool hasEditor() const {return true;}

	void enable();
	void disable();

	
private:

	const String name;

	DataThread* dataThread;
	DataBuffer* inputBuffer;

	int* numSamplesInThisBuffer;

	bool transmitData;
	bool canRelease;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SourceNode);

};


#endif  // __SOURCENODE_H_DCE798F1__

