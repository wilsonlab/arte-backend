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

FilterNode::FilterNode(const String name_, int* nSamps, const CriticalSection& lock_)
	: sampleRate (44100.0), centerFrequency(2000.0), bandWidth (200),
	  name (name_), numSamplesInThisBuffer(nSamps), lock(lock_)
	
{

	setPlayConfigDetails(16,16,44100.0,128);

	filter = new Dsp::SmoothedFilterDesign 
		<Dsp::RBJ::Design::LowPass, 16> (1024);
	
	///std::cout << "Filter created." << std::endl;

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
}

FilterNode::~FilterNode()
{
	filter = 0;
	filterEditor = 0;
}

AudioProcessorEditor* FilterNode::createEditor( )
{
	//filterEditor = new FilterEditor(this);
	
	//std::cout << "Creating editor." << std::endl;
	//filterEditor = new FilterEditor(this);
	//return filterEditor;

	return 0;
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
}

void FilterNode::releaseResources() 
{	
}

void FilterNode::processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{

	//std::cout << "FilterNode processing." << std::endl;
	 //std::cout << buffer.getNumSamples() << std::endl;
	 //std::cout << buffer.getNumChannels() << std::endl;

	//std::cout << "Filter Node sample count: " << buffer.getNumSamples() << std::endl;


	lock.enter();
	int nSamps = *numSamplesInThisBuffer;
	lock.exit();

	//std::cout << "Filter node samples: " << nSamps << std::endl;

    filter->process (nSamps, buffer.getArrayOfChannels());

   // std::cout << "Filter processing complete." << std::endl;

}
