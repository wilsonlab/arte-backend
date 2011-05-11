/*
  ==============================================================================

    GenericProcessor.h
    Created: 7 May 2011 2:26:54pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __GENERICPROCESSOR_H_1F469DAF__
#define __GENERICPROCESSOR_H_1F469DAF__


#include "../../JuceLibraryCode/JuceHeader.h"
#include <time.h>

class GenericProcessor : public AudioProcessor

{
public:
	
	// real member functions:
	GenericProcessor(const String name, int* nSamples);
	~GenericProcessor();
	
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
	double sampleRate;
	const String name;

	long int accumulator;

	time_t lastCallbackTime, thisCallbackTime;

	int* numSamplesInThisBuffer;

	//AudioProcessorEditor* editor;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GenericProcessor);

};




#endif  // __GENERICPROCESSOR_H_1F469DAF__
