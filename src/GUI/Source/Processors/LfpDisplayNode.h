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
#include "Visualization/OpenGLCanvas.h"

class DataViewport;

class LfpDisplayNode : public OpenGLCanvas,
					   public GenericProcessor

{
public:

	LfpDisplayNode();
	~LfpDisplayNode();

	void newOpenGLContextCreated();
	void renderOpenGL();
	int getTotalHeight();

	AudioProcessorEditor* createEditor();

	bool isSink() {return true;}

	void process(AudioSampleBuffer &buffer, MidiBuffer &midiMessages, int& nSamples);

	void setParameter(int, float);

	void setNumInputs(int inputs);
	void setSampleRate(float r);

	void prepareToPlay(double, int);

	bool inWindow;

private:

	DataViewport* dataViewport;
	AudioSampleBuffer* displayBuffer;
	MidiBuffer* eventBuffer;

	float timebase; // ms
	float displayGain; // 

	bool parameterChanged;


	void resizeBuffer();

	void resized();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LfpDisplayNode);

};




#endif  // __LFPDISPLAYNODE_H_D969A379__
