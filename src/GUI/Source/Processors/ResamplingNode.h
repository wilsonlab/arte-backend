/*
  ==============================================================================

    ResamplingNode.h
    Created: 7 May 2011 5:07:58pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __RESAMPLINGNODE_H_79663B0__
#define __RESAMPLINGNODE_H_79663B0__


#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Dsp/Dsp.h"

class ResamplingNode;

class ResamplingNode : public AudioProcessor

{
public:
	
	// real member functions:
	ResamplingNode(const String name_, int* nSamps, 
				   const CriticalSection& lock, bool temp);
	~ResamplingNode();
	
	const String getName() const {return name;}
	AudioSampleBuffer* getBufferAddress() { return destBuffer; }
	void updateFilter();
	
	void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock);
	void releaseResources();
	void processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages);
	void setParameter (int parameterIndex, float newValue);


	AudioProcessorEditor* createEditor( );
	bool hasEditor() const {return false;}
	// end real member functions
	
	// quick implementations of virtual functions, to be changed later:
	void reset() {}
	void setCurrentProgramStateInformation(const void* data, int sizeInBytes) {}
	void setStateInformation(const void* data, int sizeInBytes) {}
	void getCurrentProgramStateInformation(MemoryBlock &destData) {}
	void getStateInformation (MemoryBlock &destData) {}
	void changeProgramName (int index, const String &newName) {}
	void setCurrentProgram (int index) {}

	const String getInputChannelName (int channelIndex) const {return T(" ");}
	const String getOutputChannelName (int channelIndex) const {return T(" ");}
	const String getParameterName (int parameterIndex) {return T(" ");}
	const String getParameterText (int parameterIndex) {return T(" ");}
	const String getProgramName (int index) {return T(" ");}
	
	bool isInputChannelStereoPair (int index) const {return true;}
	bool isOutputChannelStereoPair (int index) const {return true;}
	bool acceptsMidi () const {return false;}
	bool producesMidi () const {return false;}

	bool isParameterAutomatable(int parameterIndex) {return false;}
	bool isMetaParameter(int parameterIndex) {return false;}
	
	int getNumParameters() {return 0;}
	int getNumPrograms() {return 0;}
	int getCurrentProgram() {return 0;}
	
	float getParameter (int parameterIndex) {return 1.0;}

private:

	// node name, obvi:
	const String name;

	// sample rate, timebase, and ratio info:
	double sourceBufferSampleRate, destBufferSampleRate;
	double ratio, lastRatio;
	double destBufferTimebaseSecs;
	int destBufferWidth;
	
	// major objects:
	Dsp::Filter* filter;
	AudioSampleBuffer* destBuffer;
	AudioSampleBuffer* tempBuffer;

	// objects for sample number synchronization:
	const CriticalSection& lock;
	int* numSamplesInThisBuffer;

	// is the destBuffer a temp buffer or not?
	bool destBufferIsTempBuffer;

	// indexing objects that persist between rounds:
	int destBufferPos;

	bool isTransmitting;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResamplingNode);

};




#endif  // __RESAMPLINGNODE_H_79663B0__
