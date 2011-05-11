/*
  ==============================================================================

    SignalGenerator.h
    Created: 9 May 2011 8:11:41pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __SIGNALGENERATOR_H_EAA44B0B__
#define __SIGNALGENERATOR_H_EAA44B0B__


#include "../../JuceLibraryCode/JuceHeader.h"

//class SourceNodeEditor;

class SignalGenerator : public AudioProcessor

{
public:
	
	// real member functions:
	SignalGenerator();
	~SignalGenerator();
	
	const String getName() const {return name;}
	
	void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock);
	void releaseResources();
	void processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

	void setParameter (int parameterIndex, float newValue);


	AudioProcessorEditor* createEditor( ) {return 0;}
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
	double frequency, sampleRate;
	double currentPhase, phasePerSample;
	float amplitude;
	//SourceNodeEditor* sourceEditor;

	const String name;
	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SignalGenerator);

};





#endif  // __SIGNALGENERATOR_H_EAA44B0B__
