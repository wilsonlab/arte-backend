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
	  sampleRate (40000.0), highCut(6000.0), lowCut(600.0)
	
{

	setNumInputs(nChans);
	setNumOutputs(nChans);

		// each family of filters is given its own namespace
		// RBJ: filters from the RBJ cookbook
		// Butterworth
		// ChebyshevI: ripple in the passband
		// ChebyshevII: ripple in the stop band
		// Elliptic: ripple in both the passband and stopband
		// Bessel: theoretically with linear phase
		// Legendre: "Optimum-L" filters with steepest transition and monotonic passband
		// Custom: Simple filters that allow poles and zeros to be specified directly

		// within each namespace exists a set of "raw filters"
		// Butterworth::LowPass
		//				HighPass
		// 				BandPass
		//				BandStop
		//				LowShelf
		// 				HighShelf
		//				BandShelf
		//
		//	class templates (such as SimpleFilter) which require FilterClasses
		//    expect an identifier of a raw filter
		//  raw filters do not support introspection, or the Params style of changing
		//    filter settings; they only offer a setup() function for updating the IIR
		//    coefficients to a given set of parameters
		//

		// each filter family namespace also has the nested namespace "Design"
		// here we have all of the raw filter names repeated, except these classes
		//  also provide the Design interface, which adds introspection, polymorphism,
		//  the Params style of changing filter settings, and in general all fo the features
		//  necessary to interoperate with the Filter virtual base class and its derived classes

		filter = new Dsp::SmoothedFilterDesign 
			<Dsp::Butterworth::Design::BandPass 	// design type
			<4>,								 	// order
			16,										// number of channels
			Dsp::DirectFormII>						// realization
			(1024);									// number of samples over which to fade 
													//   parameter changes
	
		std::cout << "Filter created." << std::endl;

		setFilterParameters();

}

FilterNode::~FilterNode()
{
	filter = 0;
}

AudioProcessorEditor* FilterNode::createEditor()
{
	FilterEditor* filterEditor = new FilterEditor(this, viewport);
	setEditor(filterEditor);
	
	std::cout << "Creating editor." << std::endl;
	//filterEditor = new FilterEditor(this);
	return filterEditor;

	//return 0;
}

//AudioProcessorEditor* FilterNode::createEditor(AudioProcessorEditor* const editor)
//{
	
//	return editor;
//}

void FilterNode::setFilterParameters()
{

	Dsp::Params params;
	params[0] = sampleRate; // sample rate
	params[1] = 4; // order
	params[2] = (highCut + lowCut)/2; // center frequency
	params[3] = highCut - lowCut; // bandwidth

	filter->setParams (params);

}

void FilterNode::setParameter (int parameterIndex, float newValue)
{
	//std::cout << "Message received." << std::endl;

	if (parameterIndex == 0) {
		lowCut = newValue;
	} else {
		highCut = newValue;
	}

	setFilterParameters();

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
	//std::cout << "Filter node processing." << std::endl;

	int nSamps = getNumSamples();
    filter->process (nSamps, buffer.getArrayOfChannels());

    //std::cout << "Filter node:" << *buffer.getSampleData(0,0);

}
