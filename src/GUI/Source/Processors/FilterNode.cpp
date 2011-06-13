/*
  ==============================================================================

    FilterNode.cpp
    Created: 7 May 2011 5:07:28pm
    Author:  jsiegle

  ==============================================================================
*/

#include <stdio.h>
#include "FilterNode.h"
//#include "FilterEditor.h"

FilterNode::FilterNode(const String name_, int* nSamps, int nChans, const CriticalSection& lock_, int id)
	: GenericProcessor(name_, nSamps, nChans, lock_, id), 
	  sampleRate (44100.0), centerFrequency(2000.0), bandWidth (200)
	
{

	setNumInputs(nChans);
	setNumOutputs(nChans);
	//setPlayConfigDetails(nChans,nChans,44100.0,128);


	//if (getName().equalsIgnoreCase("Bandpass Filter")) {

		filter = new Dsp::SmoothedFilterDesign 
			<Dsp::RBJ::Design::LowPass, 16> (1024);
	
		std::cout << "Filter created." << std::endl;

	//filter.setup(3, // order
	 //            sampleRate,
	  //           centerFrequency,
	   //          bandWidth,
	    //         1); // ripple dB

		Dsp::Params params;
		params[0] = sampleRate; // sample rate
		params[1] = centerFrequency; // cutoff frequency
		params[2] = 1.25; //Q //
		//params[3] = bandWidth; // bandWidth

		filter->setParams (params);

	//}
}

FilterNode::~FilterNode()
{
	filter = 0;
}

AudioProcessorEditor* FilterNode::createEditor()
{
	FilterEditor* filterEditor = new FilterEditor(this, viewport);
	
	std::cout << "Creating editor." << std::endl;
	//filterEditor = new FilterEditor(this);
	return filterEditor;

	//return 0;
}

//AudioProcessorEditor* FilterNode::createEditor(AudioProcessorEditor* const editor)
//{
	
//	return editor;
//}
void FilterNode::setParameter (int parameterIndex, float newValue)
{
	//std::cout << "Message received." << std::endl;
	centerFrequency = newValue;
	
	Dsp::Params params;
	params[0] = sampleRate; // sample rate
	params[1] = centerFrequency; // cutoff frequency
	params[2] = 1.25; //Q //
	//params[3] = bandWidth; // bandWidth
	
	filter->setParams (params);

}


void FilterNode::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{
	//std::cout << "Filter node preparing." << std::endl;
}

void FilterNode::releaseResources() 
{	
}

void FilterNode::processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{

	int nSamps = getNumSamples();
    filter->process (nSamps, buffer.getArrayOfChannels());

}
