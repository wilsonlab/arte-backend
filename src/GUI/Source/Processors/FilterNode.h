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

class FilterEditor;

class FilterNode : public AudioProcessor

{
public:
	
	// real member functions:
	FilterNode(const String name_, int* nSamps, const CriticalSection& lock);
	~FilterNode();
	
	const String getName() const {return name;}
	
	void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock);
	void releaseResources();
	void processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages);
	void setParameter (int parameterIndex, float newValue);


	AudioProcessorEditor* createEditor( );
	bool hasEditor() const {return false;}
	//AudioProcessorEditor* createEditor(AudioProcessorEditor* editor);
	
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
	double sampleRate, centerFrequency, bandWidth;
	const String name;
	Dsp::Filter* filter;
	FilterEditor* filterEditor;

	const CriticalSection& lock;

	int* numSamplesInThisBuffer;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterNode);

};





#endif  // __FILTERNODE_H_CED428E__
