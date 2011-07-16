/*
  ==============================================================================

    RecordNode.cpp
    Created: 10 May 2011 7:17:09pm
    Author:  jsiegle

  ==============================================================================
*/

#include "RecordNode.h"

RecordNode::RecordNode(const String name_,
					   int* nSamples, int numChans,
					   const CriticalSection& lock_,
					   int id)
	: GenericProcessor(name_, nSamples, numChans, lock_, id), isRecording(false)
{

	setPlayConfigDetails(numChans,0,44100.0,128);

	outputFile = File("./data"); // create output file
	outputStream = 0;
}


RecordNode::~RecordNode() {

}


void RecordNode::setParameter (int parameterIndex, float newValue)
{
 	if (parameterIndex == 1) {
 		isRecording = true;
 	} else {
 		isRecording = false;
 	}
}


void RecordNode::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{
	
	outputStream = outputFile.createOutputStream();
}

void RecordNode::releaseResources() 
{	
	
	if (outputStream != 0) {
		outputStream->flush();

		delete(outputStream);
		outputStream = 0;
	}
}

float RecordNode::getFreeSpace()
{
	return (1.0f-float(outputFile.getBytesFreeOnVolume())/float(outputFile.getVolumeTotalSize()));
}

void RecordNode::processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{

	//std::cout << "Record node processing block." << std::endl;
	//std::cout << "Num channels: " << buffer.getNumChannels() << std::endl;

	if (isRecording) {

		int nSamps = getNumSamples();

		for (int n = 0; n < nSamps; n++) {
		
			float* sample = buffer.getSampleData(1,n);
			outputStream->writeFloat(*sample);
			//AudioDataConverters::convertFloatToInt16BE(&sample)
			//);
		}
	}

}
