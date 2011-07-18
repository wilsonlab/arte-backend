/*
  ==============================================================================

    GenericProcessor.cpp
    Created: 7 May 2011 2:26:54pm
    Author:  jsiegle

  ==============================================================================
*/

#include "GenericProcessor.h"

GenericProcessor::GenericProcessor(const String name_, int* nSamps, int nChans, const CriticalSection& lock_, int id)
	: numSamplesInThisBuffer(nSamps),
	  name (name_), lock(lock_), nodeId(id), numInputs(nChans), numOutputs(nChans),
	  sourceNode(0), destNode(0)
{

	setPlayConfigDetails(numInputs,numOutputs,44100.0,*nSamps);

}

GenericProcessor::~GenericProcessor()
{
	//deleteAllChildren();
}

AudioProcessorEditor* GenericProcessor::createEditor()
{
	
	return new GenericEditor (this, viewport);
}


void GenericProcessor::setParameter (int parameterIndex, float newValue)
{


}

void GenericProcessor::setViewport(FilterViewport* vp) {
	
	viewport = vp;
}


void GenericProcessor::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{
	//std::cout << "Preparing to play." << std::endl;

}

void GenericProcessor::releaseResources() 
{	
}


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

int GenericProcessor::getNumInputs() {
	return numInputs;
}

void GenericProcessor::setNumInputs(int n) {
	numInputs = n;
	setPlayConfigDetails(numInputs,numOutputs,44100.0,1024);
}

int GenericProcessor::getNumOutputs() {
	return numOutputs;
}

void GenericProcessor::setNumOutputs(int n) {
	numOutputs = n;
	setPlayConfigDetails(numInputs,numOutputs,44100.0,1024);
}



int GenericProcessor::getNodeId() {
	
	return nodeId;

}



void GenericProcessor::processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
	
	// this method must be implemented by all subclasses
	 
}
