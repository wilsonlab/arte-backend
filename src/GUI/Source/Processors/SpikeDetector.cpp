/*
  ==============================================================================

    SpikeDetector.cpp
    Created: 14 Aug 2011 3:36:00pm
    Author:  jsiegle

  ==============================================================================
*/

#include <stdio.h>
#include "SpikeDetector.h"

SpikeDetector::SpikeDetector(const String name_, int* nSamps, int nChans, const CriticalSection& lock_, int id)
	: GenericProcessor(name_, nSamps, nChans, lock_, id), 
	  sampleRate (40000.0), threshold(100), prePeakMs(0.2), postPeakMs(0.8),
	  accumulator(0), spikeBuffer(0)
	
{

	setNumInputs(nChans);
	setNumOutputs(nChans);

}

SpikeDetector::~SpikeDetector()
{

}


//AudioProcessorEditor* SpikeDetector::createEditor(AudioProcessorEditor* const editor)
//{
	
//	return editor;
//}

void SpikeDetector::setParameter (int parameterIndex, float newValue)
{
	//std::cout << "Message received." << std::endl;

}


void SpikeDetector::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{
	//std::cout << "Filter node preparing." << std::endl;
}

void SpikeDetector::releaseResources() 
{	
}

void SpikeDetector::processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
	//std::cout << "Filter node processing." << std::endl;

    //std::cout << "Filter node:" << *buffer.getSampleData(0,0);

    accumulator++;

    if (accumulator > 20) {

    		std::cout << "  >> New event." << std::endl;

			uint8 data[95];

			for (int n = 0; n < sizeof(data); n++) {
				data[n] = 1;
			}

			//MidiMessage event = MidiMessage::noteOn(2,1,10.0f);
			MidiMessage event = MidiMessage(data, 	// spike data (float)
											sizeof(data), 	// number of bytes to use
											1000.0 	// timestamp (64-bit)
											);
			
			//event.setChannel(1);

			midiMessages.addEvent(data, sizeof(data), 5);
			//midiMessages.addEvent(event, 1);

			for (int n = 0; n < sizeof(data); n++) {
				data[n] = 2;
			}

			midiMessages.addEvent(data, sizeof(data), 10);

			for (int n = 0; n < sizeof(data); n++) {
				data[n] = 3;
			}

			midiMessages.addEvent(data, sizeof(data), 15);

			//midiMessages.addEvent(event, 5);

			//std::cout << "Midi buffer contains " << midiMessages.getNumEvents() << " events." << std::endl;

			accumulator = 0;
	}

}
