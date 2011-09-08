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
	  sourceNode(0), destNode(0), editor(0)
{

	setPlayConfigDetails(numInputs,numOutputs,44100.0,*nSamps);

}

GenericProcessor::~GenericProcessor()
{
	//deleteAllChildren();
	//std::cout << name << " deleting editor." << std::endl;

	if (editor != 0)
	{
		delete(editor);
		editor = 0;
	}
}

AudioProcessorEditor* GenericProcessor::createEditor()
{
	editor = new GenericEditor (this, viewport); 
	return editor;
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

// void GenericProcessor::setSourceNode(GenericProcessor* sn)
// {
// 	if (!isSource())
// 		sourceNode = sn;
// 	else
// 		sourceNode = 0;
// }

// void GenericProcessor::setDestNode(GenericProcessor* dn)
// {
// 	if (!isSink())
// 		destNode = dn
// 	else
// 		destNode = 0;
// }

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
