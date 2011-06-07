/*
  ==============================================================================

    NetworkNode.h
    Created: 27 May 2011 1:29:49pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __NETWORKNODE_H_50230508__
#define __NETWORKNODE_H_50230508__

#include "../../JuceLibraryCode/JuceHeader.h"
#include <stdio.h>

#include "unp.h"
#include "netcom.h"
#include "datapacket.h"
#include "../Processors/DataBuffer.h"

class NetworkThread;

class NetworkNode : public AudioProcessor
{
public:
	
	// real member functions:
	NetworkNode(const String name, int* nSamples, const CriticalSection& lock);
	~NetworkNode();

	void start();
	void stop();
	
	const String getName() const {return name;}
	
	void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock);
	void releaseResources();
	void processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

	void setParameter (int parameterIndex, float newValue);

	AudioProcessorEditor* createEditor( ) {return 0;}
	bool hasEditor() const {return false;}

	// end real member functions
	
	// quick implementations of virtual functions, to be changed later:
	void reset() {}
	void setCurrentProgramStateInformation(const void* data, int sizeInBytes) {}
	void setStateInformation(const void* data, int sizeInBytes) {}
	void getCurrentProgramStateInformation(MemoryBlock &destData) {}
	void getStateInformation (MemoryBlock &destData) {}
	void changeProgramName (int index, const String &newName) {}
	void setCurrentProgram (int index) {}

	const String getInputChannelName (int channelIndex) const {return T(" ");}
	const String getOutputChannelName (int channelIndex) const {return T(" ");}
	const String getParameterName (int parameterIndex) {return T(" ");}
	const String getParameterText (int parameterIndex) {return T(" ");}
	const String getProgramName (int index) {return T(" ");}
	
	bool isInputChannelStereoPair (int index) const {return true;}
	bool isOutputChannelStereoPair (int index) const {return true;}
	bool acceptsMidi () const {return false;}
	bool producesMidi () const {return false;}
	bool isParameterAutomatable(int parameterIndex) {return false;}
	bool isMetaParameter(int parameterIndex) {return false;}
	
	int getNumParameters() {return 0;}
	int getNumPrograms() {return 0;}
	int getCurrentProgram() {return 0;}
	
	float getParameter (int parameterIndex) {return 1.0;}
	
private:

	const String name;

    //int playRequest;
    bool transmitData;
	
	NetworkThread* networkThread;
	DataBuffer* inputBuffer;

	int* numSamplesInThisBuffer;

	const CriticalSection& lock;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NetworkNode);

};


// DEFINE NETWORK THREAD:

class NetworkThread : public Thread
{

public:
	NetworkThread() : Thread ("Network Thread")
	{
		char host[] = "10.121.43.47";
		char port[] = "5227";

		my_netcomdat = my_netcom.initUdpRx(host, port);
		
		dataBuffer = new DataBuffer(8, 4096);

		startThread();

		std::cout << "Network interface created." << std::endl;
	}

	~NetworkThread() {
		stopThread(500);
		close(my_netcomdat.sockfd);
		// need to close socket in order to reopen
		close(my_netcomdat.sockfd);

		std::cout << "Network interface destroyed." << std::endl;

		delete dataBuffer;
		dataBuffer = 0;
	}

	void run() {

		while (! threadShouldExit())
		{
			const MessageManagerLock mml (Thread::getCurrentThread());
			if (! mml.lockWasGained())
				return;
			updateBuffer();
		}

	}


	DataBuffer* getBufferAddress() {
		return dataBuffer;
	}

private:

	NetCom my_netcom;
	NetComDat my_netcomdat;

	lfp_bank_net_t lfp;

	DataBuffer* dataBuffer;

	float thisSample[8];

	void updateBuffer(){
		
	 	NetCom::rxWave (my_netcomdat, &lfp);

	 	for (int s = 0; s < lfp.n_samps_per_chan; s++) {
	 		for (int c = 0; c < lfp.n_chans; c++) {
	 			thisSample[c] = float(lfp.data[s*lfp.n_chans + c])/500.0f;
	 		}
	 		dataBuffer->addToBuffer(thisSample,1);
	 	}
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NetworkThread);


};



#endif  // __NETWORKNODE_H_50230508__
