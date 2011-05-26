/*
  ==============================================================================

    AudioComponent.cpp
    Created: 7 May 2011 1:35:05pm
    Author:  jsiegle

  ==============================================================================
*/

#include "AudioComponent.h"
#include <stdio.h>

AudioComponent::AudioComponent() 
{
	
	initialise(0,  // numInputChannelsNeeded
			   2,  // numOutputChannelsNeeded
			   0,  // *savedState (XmlElement)
			   true, // selectDefaultDeviceOnFailure
			   String::empty, // preferred device
			   0); // preferred device setup options
	
	AudioIODevice* aIOd = getCurrentAudioDevice();

	String devType = getCurrentAudioDeviceType();
	//std::cout << "Device type A: " << devType << std::endl;

	String devName = aIOd->getName();
	
	std::cout << std::endl << "Audio device name: " << devName << std::endl;

	AudioDeviceManager::AudioDeviceSetup setup;
	getAudioDeviceSetup(setup);

	setup.bufferSize = 2048; /// larger buffer = fewer empty blocks, but longer latencies
	setup.useDefaultInputChannels = false;
	setup.inputChannels = 0;
	setup.useDefaultOutputChannels = true;
	setup.outputChannels = 2;
	setup.sampleRate = 44100.0;

	String msg = setAudioDeviceSetup(setup, false);

	//std::cout << "Message: " << msg << std::endl;

	devType = getCurrentAudioDeviceType();
	std::cout << "Device type: " << devType << std::endl;

	//AudioIODevice* aiod = get

	//getAudioDeviceSetup(setup);

	float sr = setup.sampleRate;
	int buffSize = setup.bufferSize;
	String oDN = setup.outputDeviceName;
	BigInteger oC = setup.outputChannels;
	//BigInteger iChan = setup.inputChannels;
	//BigInteger oChan = setup.outputChannels;

	std::cout << "Audio output device: " <<  oDN << std::endl;
	std::cout << "Audio output channels: " <<  oC.toInteger() << std::endl;
	std::cout << "Audio device sample rate: " <<  sr << std::endl;
	std::cout << "Audio device buffer size: " << buffSize << std::endl << std::endl;
	//std::cout << "Audio device input channels: " << iChan << std::endl;
	//std::cout << "Audio device output channels: " << oChan << std::endl;

	//std::cout << this->getOutputChannelName(0) << std::endl;
	//std::cout << this->isOutputChannelStereoPair(0) << std::endl;

	graphPlayer = new AudioProcessorPlayer();

}

AudioComponent::~AudioComponent() {
	
	endCallbacks();
	//AudioProcessor* graph = graphPlayer->getCurrentProcessor();
	//deleteAndZero(graph);
	deleteAndZero(graphPlayer);
	//deleteAllChildren();

}

void AudioComponent::connectToProcessorGraph(AudioProcessorGraph* processorGraph)
{
	
	graphPlayer->setProcessor(processorGraph);

}

void AudioComponent::disconnectProcessorGraph()
{
	
	graphPlayer->setProcessor(0);

}

void AudioComponent::beginCallbacks() {
	
	std::cout << std::endl << "Adding audio callback." << std::endl;
	addAudioCallback(graphPlayer);

}

void AudioComponent::endCallbacks() {
	
	std::cout << std::endl << "Removing audio callback." << std::endl;
	removeAudioCallback(graphPlayer);

}

