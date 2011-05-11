/*
  ==============================================================================

    GenericProcessor.cpp
    Created: 7 May 2011 2:26:54pm
    Author:  jsiegle

  ==============================================================================
*/

#include <stdio.h>
#include "GenericProcessor.h"

GenericProcessor::GenericProcessor(const String name_, int* nSamps)
	: sampleRate (44100.0), accumulator(0), numSamplesInThisBuffer(nSamps),
	  name (name_)
{

	setPlayConfigDetails(16,16,44100.0,128);

}

GenericProcessor::~GenericProcessor()
{
	//deleteAllChildren();
}

AudioProcessorEditor* GenericProcessor::createEditor( )
{
	//filterEditor = new FilterEditor(this);
	
	//std::cout << "Creating editor." << std::endl;
	//AudioProcessorEditor* editor = new AudioProcessorEditor(this);
	return 0;
}


void GenericProcessor::setParameter (int parameterIndex, float newValue)
{


}


void GenericProcessor::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{
	//std::cout << "Preparing to play." << std::endl;
	time(&lastCallbackTime);

}

void GenericProcessor::releaseResources() 
{	
}

void GenericProcessor::processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
	
	/*++accumulator;
	//std::cout << "  ja" << std::endl;

	if (accumulator > 1000) {
		
		time(&thisCallbackTime);

		double dif = difftime(thisCallbackTime, lastCallbackTime);
		int nChans = buffer.getNumChannels();
		int nSamps = buffer.getNumSamples();

		//std::cout << name << std::endl;
		//std::cout.precision(5);
		//std::cout << "  Time difference for 1000 callbacks: " << dif << std::endl;
		//std::cout << "  Buffer size: " << nSamps << " samples x " 
		// 		  					 << nChans << " channels." 
		// 		  					 << std::endl;

		time(&lastCallbackTime);

		accumulator = 0;

	}*/
	 
}
