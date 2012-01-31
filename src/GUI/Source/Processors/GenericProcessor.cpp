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

// void GenericProcessor::setViewport(FilterViewport* vp) {
	
// 	viewport = vp;
// }


void GenericProcessor::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{
	//std::cout << "Preparing to play." << std::endl;

}

void GenericProcessor::releaseResources() 
{	
}


void GenericProcessor::setNumSamples(MidiBuffer& midiMessages, int numberToAdd) {
	//lock.enter();
	//*numSamplesInThisBuffer = n;
	//lock.exit();

	uint8 data[2];

	data[0] = numberToAdd >> 8; 	// most-significant byte
    data[1] = numberToAdd & 0xFF; 	// least-significant byte

    midiMessages.addEvent(data, 		// spike data
                          sizeof(data), // total bytes
                          0);           // sample index


}

int GenericProcessor::getNumSamples(MidiBuffer& midiMessages) {
	//lock.enter();
	//int numRead = *numSamplesInThisBuffer;
	//lock.exit();
	int numRead = 0;

	if (midiMessages.getNumEvents() > 0) 
	{
			
		int m = midiMessages.getNumEvents();

		// if (m == 1)
		// 	std::cout << m << " event received by node " << getNodeId() << std::endl;
		// else
		// 	std::cout << m << " events received by node " << getNodeId() << std::endl;

		MidiBuffer::Iterator i (midiMessages);
		MidiMessage message(0xf4);

		int samplePosition = 0;
		//i.setNextSamplePosition(samplePosition);

		while (i.getNextEvent (message, samplePosition)) {
			
				int numbytes = message.getRawDataSize();
				uint8* dataptr = message.getRawData();

				//std::cout << " Bytes received: " << numbytes << std::endl;
				//std::cout << " Message timestamp = " << message.getTimeStamp() << std::endl;

				numRead = (*dataptr<<8) + *(dataptr+1);
				//std::cout << "   " << numRead << std::endl;
		}

	}

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


void GenericProcessor::setNumInputs(int n) {
	numInputs = n;
	setPlayConfigDetails(numInputs,numOutputs,44100.0,1024);
}

void GenericProcessor::setNumOutputs(int n) {
	numOutputs = n;
	setPlayConfigDetails(numInputs,numOutputs,44100.0,1024);
}

void GenericProcessor::checkForMidiEvents(MidiBuffer& midiMessages)
{

	if (midiMessages.getNumEvents() > 0) 
	{
			
		int m = midiMessages.getNumEvents();
		std::cout << m << "events received by node " << getNodeId() << std::endl;

		MidiBuffer::Iterator i (midiMessages);
		MidiMessage message(0xf4);

		int samplePosition;
		i.setNextSamplePosition(samplePosition);

		while (i.getNextEvent (message, samplePosition)) {
			
				int numbytes = message.getRawDataSize();
				uint8* dataptr = message.getRawData();

				std::cout << " Bytes received: " << numbytes << std::endl;
				std::cout << " Message timestamp = " << message.getTimeStamp() << std::endl;

				int value = (*dataptr<<8) + *(dataptr+1);
				std::cout << "   " << value << std::endl;
		}

	}
}

void GenericProcessor::addMidiEvent(MidiBuffer& midiMessages, int numberToAdd)
{
	uint8 data[2];

	data[0] = numberToAdd >> 8; 	// most-significant byte
    data[1] = numberToAdd & 0xFF; 	// least-significant byte

    midiMessages.addEvent(data, 		// spike data
                          sizeof(data), // total bytes
                          0);           // sample index
}

void GenericProcessor::processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
	
	// this method must be implemented by all subclasses


	 
}
