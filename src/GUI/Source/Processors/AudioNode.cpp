/*
  ==============================================================================

    AudioNode.cpp
    Created: 14 Jul 2011 6:04:39pm
    Author:  jsiegle

  ==============================================================================
*/


#include "AudioNode.h"

AudioNode::AudioNode(const String name_,
					   int* nSamples, int numChans,
					   const CriticalSection& lock_,
					   int id)
	: GenericProcessor(name_, nSamples, numChans, lock_, id)
{

	// 1024 inputs, 2 outputs (left and right channel)
	setPlayConfigDetails(numChans,2,44100.0,128);

	leftChan.add(0);
	rightChan.add(1);
}


AudioNode::~AudioNode() {

}

AudioProcessorEditor* AudioNode::createEditor()
{
	
	AudioEditor* editor = new AudioEditor(this);
	
	return editor; 

}


void AudioNode::setParameter (int parameterIndex, float newValue)
{
	// change left channel, right channel, or volume

}


void AudioNode::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{

}

void AudioNode::releaseResources() 
{	

}



void AudioNode::processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{

	buffer.clear(0,0,buffer.getNumSamples());
	buffer.clear(1,0,buffer.getNumSamples());

	for (int n = 0; n < leftChan.size(); n++) {
		buffer.addFrom(0,  // destination channel
					   0,  // destination start sample
					   buffer,      // source
					   leftChan[n]+2, // source channel
					   0,           // source start sample
					   buffer.getNumSamples(), //  number of samples
					   volume       // gain to apply
					   );
	}
	
	for (int n = 0; n < rightChan.size(); n++) {
		buffer.addFrom(1,  // destination channel
					   0,  // destination start sample
					   buffer,      // source
					   rightChan[n]+2, // source channel
					   0,           // source start sample
					   buffer.getNumSamples(), //  number of samples
					   volume       // gain to apply
					   );
	}
}
