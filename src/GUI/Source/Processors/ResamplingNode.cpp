/*
  ==============================================================================

    ResamplingNode.cpp
    Created: 7 May 2011 5:07:58pm
    Author:  jsiegle

  ==============================================================================
*/

#include "ResamplingNode.h"
#include <stdio.h>

ResamplingNode::ResamplingNode (const String name_, int* nSamps, 
			      const CriticalSection& lock_, bool temp)
	: ratio (1.0), lastRatio (1.0),
	  name (name_), numSamplesInThisBuffer(nSamps), lock(lock_),
	  destBufferPos(0), destBufferIsTempBuffer(temp),
	  destBufferSampleRate(44100.0), sourceBufferSampleRate(25000.0)
	
{

	setPlayConfigDetails(16,16,44100.0,128);

	filter = new Dsp::SmoothedFilterDesign 
		<Dsp::RBJ::Design::LowPass, 1> (1024);

	if (destBufferIsTempBuffer) 
		destBufferWidth = 256;
	else
		destBufferWidth = 1000;

	destBufferTimebaseSecs = 1.0;

	destBuffer = new AudioSampleBuffer(16, destBufferWidth);
	tempBuffer = new AudioSampleBuffer(16, destBufferWidth);
	

	// filter->getKind()
	// filter->getName()
	// filter->getNumParams()
	// filter->getParamInfo()
	// filter->getDefaultParams()
	// filter->getParams()
	// filter->getParam()

	// filter->setParam()
	// filter->findParamId()
	// filter->setParamById()
	// filter->setParams()
	// filter->copyParamsFrom()

	// filter->getPoleZeros()
	// filter->response()
	// filter->getNumChannels()
	// filter->reset()
	// filter->process()

	// Filter families:
	// RBJ: from RBJ cookbook (audio-specific)
	// Butterworth
	// ChebyshevI: ripple in the passband
	// ChebyshevII: ripple in the stopband
	// Elliptic: ripple in passband and stopband
	// Bessel: theoretically with linear phase
	// Legendre: steepest transition and monotonic passband
	// Custom: poles and zeros can be specified directly

	// Filter classes:
	// vary by filter family
}

ResamplingNode::~ResamplingNode()
{
	filter = 0;
	//filterEditor = 0;
}

AudioProcessorEditor* ResamplingNode::createEditor( )
{
	//filterEditor = new FilterEditor(this);
	
	//std::cout << "Creating editor." << std::endl;
	//filterEditor = new FilterEditor(this);
	//return filterEditor;

	return 0;
}


void ResamplingNode::setParameter (int parameterIndex, float newValue)
{

	switch (parameterIndex) {
		
		case 0: destBufferTimebaseSecs = newValue; break;
		case 1: destBufferWidth = roundToInt(newValue);

	}

	// reset to zero and clear if timebase or width has changed.
	destBufferPos = 0; 
	destBuffer->clear(); 

}


void ResamplingNode::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{


	if (destBufferIsTempBuffer) {
		destBufferSampleRate = sampleRate_;
		tempBuffer->setSize(16, estimatedSamplesPerBlock);
	}
	else {
		destBufferSampleRate = float(destBufferWidth) / destBufferTimebaseSecs;
		destBuffer->setSize(16, destBufferWidth);
	}

	destBuffer->clear();

	updateFilter();

}

void ResamplingNode::updateFilter() {
	
	double cutoffFreq = (ratio > 1.0) ? 2 * destBufferSampleRate  // downsample
									  : destBufferSampleRate / 2; // upsample

    double sampleFreq = (ratio > 1.0) ? sourceBufferSampleRate // downsample
    								  : destBufferSampleRate;  // upsample
								
	Dsp::Params params;
	params[0] = sampleFreq; // sample rate
	params[1] = cutoffFreq; // cutoff frequency
	params[2] = 1.25; //Q //

	filter->setParams (params);
	
}

void ResamplingNode::releaseResources() 
{	
	deleteAndZero(destBuffer);
	deleteAndZero(tempBuffer);
}

void ResamplingNode::processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{

	lock.enter();
	int nSamps = *numSamplesInThisBuffer;
	lock.exit();


   // for (int n = 0; n < buffer.getNumSamples(); n+= 10)
    //	std::cout << buffer.getMagnitude(1,n,1) << " ";
    
    //std::cout << "END OF OLD BUFFER." << std::endl;

	if (destBufferIsTempBuffer)
		ratio = float(nSamps) / float(buffer.getNumSamples());
	else
		ratio = sourceBufferSampleRate / destBufferSampleRate;

	//std::cout << ratio << std::endl;

	if (lastRatio != ratio) {
		updateFilter();
		lastRatio = ratio;
	}


	if (ratio > 1.0001) {
		// pre-apply filter before downsampling
		filter->process (nSamps, buffer.getArrayOfChannels());
	}


	// initialize variables
	tempBuffer->clear();
	int sourceBufferPos = 0;
	int sourceBufferSize = buffer.getNumSamples();
	float subSampleOffset = 0.0;
	int nextPos = (sourceBufferPos + 1) % sourceBufferSize;

	int tempBufferPos = 0;
	//int totalSamples = 0;

	// code modified from "juce_ResamplingAudioSource.cpp":

    for (int tempBufferPos = 0; tempBufferPos < tempBuffer->getNumSamples(); tempBufferPos++)
    {
    	float gain = 1.0;
        float alpha = (float) subSampleOffset;
        float invAlpha = 1.0f - alpha;

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {

        	tempBuffer->addFrom(channel, 		// destChannel
        						tempBufferPos,  // destSampleOffset
        						buffer,			// source
        						channel,		// sourceChannel
        						sourceBufferPos,// sourceSampleOffset
        						1,				// number of samples
        						invAlpha*gain);      // gain to apply to source
        	
        	tempBuffer->addFrom(channel, 		// destChannel
        					   tempBufferPos,   // destSampleOffset
        					   buffer,			// source
        					   channel,			// sourceChannel
        					   nextPos,		 	// sourceSampleOffset
        					   1,				// number of samples
        					   alpha*gain);     	 // gain to apply to source

       	}

       	//++tempBufferPos;

        subSampleOffset += ratio;

        while (subSampleOffset >= 1.0)
        {
            if (++sourceBufferPos >= sourceBufferSize)
                sourceBufferPos = 0;

            nextPos = (sourceBufferPos + 1) % sourceBufferSize;
            subSampleOffset -= 1.0;
        }
    }


	if (ratio < 0.9999) {

		filter->process (tempBufferPos, tempBuffer->getArrayOfChannels());
		// apply the filter after upsampling
		//filter->process (totalSamples, buffer.getArrayOfChannels());
	} else if (ratio <= 1.0001) {
		
		// no resampling is being applied, no need to filter, BUT...
		// keep the filter stoked with samples to avoid discontinuities

	}

	if (destBufferIsTempBuffer) {
    	
    	// copy the temp buffer into the original buffer
    	buffer = *tempBuffer;

    	//buffer.setSize(2,0,true,false,true);

    	//for (int n = 0; n < buffer.getNumSamples(); n+= 10)
    	//std::cout << buffer.getRMSLevel(1,0,buffer.getNumSamples()) << " ";
    
    	//std::cout << "END OF NEW BUFFER." << std::endl;

    } else {
    	
    	// copy the temp buffer into the destination buffer
    	for (int channel = 0; channel < destBuffer->getNumChannels(); channel++)
    		destBuffer->copyFrom(channel,destBufferPos,*tempBuffer,channel,0,tempBufferPos);
    
    	destBufferPos += tempBufferPos;
    	destBufferPos %= destBufferWidth;

    }


}