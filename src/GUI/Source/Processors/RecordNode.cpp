/*
  ==============================================================================

    RecordNode.cpp
    Created: 10 May 2011 7:17:09pm
    Author:  jsiegle

  ==============================================================================
*/

#include "RecordNode.h"


RecordNode::RecordNode(const String name_, int* nSamples, const CriticalSection& lock_)
	: name(name_), numSamplesInThisBuffer(nSamples), lock(lock_)
{
	outputFile = File("./data");
	outputStream = outputFile.createOutputStream();

	setPlayConfigDetails(3,0,44100.0,128);
}


RecordNode::~RecordNode() {
	
	outputStream->flush();
	delete(outputStream);
	outputStream = 0;

}


void RecordNode::setParameter (int parameterIndex, float newValue)
{


}


void RecordNode::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{

}

void RecordNode::releaseResources() 
{	
}

void RecordNode::processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{

	//std::cout << "Record node processing block." << std::endl;
	//std::cout << "Num channels: " << buffer.getNumChannels() << std::endl;

	lock.enter();
	int nSamps = *numSamplesInThisBuffer;
	lock.exit();

	for (int n = 0; n < nSamps; n++) {
		
		float* sample = buffer.getSampleData(1,n);
		outputStream->writeFloat(*sample);
			//AudioDataConverters::convertFloatToInt16BE(&sample)
			//);
	}

	//for (int n = 0; n < 1; n++) {
	//	outputStream->writeByte(1);
	//}

}
	 //std::c