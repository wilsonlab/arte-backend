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
	  sampleRate (40000.0), threshold(5000.0), prePeakMs(0.2), postPeakMs(0.6),
	  accumulator(0)
	
{

	setNumInputs(nChans);
	setNumOutputs(nChans);

	spikeBuffer = new MidiBuffer();

}

SpikeDetector::~SpikeDetector()
{
	deleteAndZero(spikeBuffer);
}


AudioProcessorEditor* SpikeDetector::createEditor()
{

	SpikeDetectorEditor* editor = new SpikeDetectorEditor(this, viewport);
	
	std::cout << "Creating editor." << std::endl;

    setEditor(editor);

	return editor;
}

void SpikeDetector::setParameter (int parameterIndex, float newValue)
{
	//std::cout << "Message received." << std::endl;
	if (parameterIndex == 0) {
		threshold = newValue;
	}

}


void SpikeDetector::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{
	//std::cout << "SpikeDetector node preparing." << std::endl;
	prePeakSamples = int((prePeakMs / 1000.0f) / (1/sampleRate));
	postPeakSamples = int((postPeakMs / 1000.0f) / (1/sampleRate));
}

void SpikeDetector::releaseResources() 
{	
}

void SpikeDetector::processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{

	int maxSamples = getNumSamples();
	int spikeSize = 2 + prePeakSamples*2 + postPeakSamples*2; 
    
    for (int chan = 1; chan < 2; chan++) 
    {

    	int n = prePeakSamples + 1; // 1-sample buffer, just to be safe

    	// TODO: save end of previous buffer to check for spikes at border

    	while (n < maxSamples - postPeakSamples - 1) {
    		
    		// search through to find spikes
    		if (*buffer.getSampleData(chan,n) > threshold) {
    			
    			uint8 data[spikeSize];

    			data[0] = chan & 070; // channel most-significant byte
    			data[1] = chan & 007; // channel least-significant byte

    			// not currently looking for peak, just centering on thresh cross
    			//converter.convertSamples(data+2, //dest
    			//					     buffer.getSampleData(chan, n - prePeakSamples), // source
    			//					     prePeakSamples + postPeakSamples); // numSamples

    			//std::cout << *buffer.getSampleData(chan, n) << std::endl;

    			uint8* dataptr = data+2;

    			for (int sample = -prePeakSamples; sample < postPeakSamples; sample++) {
    				
    				uint16 sampleValue = uint16(*buffer.getSampleData(chan, n+sample) + 32768);

    				*dataptr++ = uint8(sampleValue >> 8);
    				*dataptr++ = uint8(sampleValue & 255);

    			}
    			
    			//AudioDataConverters::convertFloatToInt16BE ( buffer.getSampleData(chan, n - prePeakSamples), // source
    			//										 data+2, // dest
    			//										 prePeakSamples + postPeakSamples, // numSamples
    			//										 2 ); // destBytesPerSample = 2

				spikeBuffer->addEvent(data, 		// spike data
								  sizeof(data), // total bytes
								  n); 			// sample index

    			n += postPeakSamples;

    		} else {
    			n += 1;
    		}

    	}
    }

 //    accumulator++;


 //    if (accumulator > 20) {

 //    		std::cout << "  >> New event." << std::endl;

 //    		uint8 data[32];

 //    		data[0] = 0; // channel most-significant byte
 //    		data[1] = 1; // channel least-significant byte
 
 //    		AudioDataConverters::convertFloatToInt16BE ( buffer.getSampleData(1), // source
 //    													 data+2, // dest
 //    													 15 , // numSamples
 //    													 2 ); // destBytesPerSample = 2

	// 		spikeBuffer->addEvent(data, 		// spike data
	// 							  sizeof(data), // total bytes
	// 							  5); 			// sample index

	// 		accumulator = 0;
	// }

}
