/*
  ==============================================================================

    GenericProcessor.cpp
    Created: 7 May 2011 2:26:54pm
    Author:  jsiegle

  ==============================================================================
*/

#include "GenericProcessor.h"

GenericProcessor::GenericProcessor(const String name_, int* nSamps, int nChans, const CriticalSection& lock_)
	: numSamplesInThisBuffer(nSamps),
	  name (name_), lock(lock_)
{

	setPlayConfigDetails(nChans,nChans,44100.0,*nSamps);

}

GenericProcessor::~GenericProcessor()
{
	//deleteAllChildren();
}

AudioProcessorEditor* GenericProcessor::createEditor( )
{
	
	return new GenericEditor (this);
}


void GenericProcessor::setParameter (int parameterIndex, float newValue)
{


}


void GenericProcessor::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{
	//std::cout << "Preparing to play." << std::endl;
	//time(&lastCallbackTime);

}

void GenericProcessor::releaseResources() 
{	
}

// void GenericProcessor::lock() {
// 	myLock.enter();
// }

// void GenericProcessor::unlock(){
// 	myLock.exit();
// }

void GenericProcessor::setNumSamples(int n) {
	lock.enter();
	*numSamplesInThisBuffer = n;
	lock.exit();
}

int GenericProcessor::getNumSamples() {
	lock.enter();
	int numRead = *numSamplesInThisBuffer;
	lock.exit();

	return numRead;
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
