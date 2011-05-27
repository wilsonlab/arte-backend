/*
  ==============================================================================

    SourceNode.h
    Created: 7 May 2011 5:07:14pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __SOURCENODE_H_DCE798F1__
#define __SOURCENODE_H_DCE798F1__



#include "../../JuceLibraryCode/JuceHeader.h"
#include <ftdi.h>
#include <stdio.h>
#include "DataBuffer.h"

class DataThread;

class SourceNode : public AudioProcessor
{
public:
	
	// real member functions:
	SourceNode(const String name, int* nSamples, const CriticalSection& lock);
	~SourceNode();

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
	
	DataThread* dataThread;
	DataBuffer* inputBuffer;

	int* numSamplesInThisBuffer;

	const CriticalSection& lock;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SourceNode);

};


// DEFINE DATA THREAD:

class DataThread : public Thread
{

public:
	DataThread() : Thread ("Data Thread"),
			vendorID(0x0403),
			productID(0x6010),
			baudrate(115200),
			startCode(83),
			stopCode(115),
			isRunning(false),
			ch(-1),
			isTransmitting(false)

	{
		ftdi_init(&ftdic);
   		ftdi_set_interface(&ftdic, INTERFACE_ANY);
		ftdi_usb_open(&ftdic, vendorID, productID);
		ftdi_set_baudrate(&ftdic, baudrate);

		std::cout << "FTDI interface initialized." << std::endl;

		ftdi_write_data(&ftdic, &startCode, 1);

		dataBuffer = new DataBuffer(16, 4096);

		startThread();

		isTransmitting = true;
	}

	~DataThread() {

		stopThread(500);

			ftdi_write_data(&ftdic, &stopCode, 1);
			unsigned char buf[4097]; // has to be bigger than the on-chip buffer
			ftdi_read_data(&ftdic, buf, sizeof(buf));

			ftdi_usb_close(&ftdic);
    		ftdi_deinit(&ftdic);
			std::cout << "FTDI interface destroyed." << std::endl;

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

	void start() {
		std::cout << "I'm gonna start now." << std::endl;
		
		const ScopedLock sl (lock);

		//std::cout << "Created ." << std::endl;
		

		isTransmitting = true;
		std::cout << "I started." << std::endl;
	}

	void stop() {

		std::cout << "I'm gonna stop now." << std::endl;
		
		const ScopedLock sl (lock);
		isTransmitting = false;
		
	}

	DataBuffer* getBufferAddress() {
		return dataBuffer;
	}

private:

	struct ftdi_context ftdic;
	int vendorID, productID;
	int baudrate;
	bool isTransmitting;
	
	unsigned char startCode, stopCode;
	unsigned char buffer[240]; // should be 5 samples per channel

	bool isRunning;
	bool startSaving;

	DataBuffer* dataBuffer;

	short int thisSampleInt16;
	float thisSample[16];

	int ch;
	int accumulator;

	CriticalSection lock;

	bool getStatus() {
		const ScopedLock sl (lock);
		return isTransmitting;
		
	}

	//short int data[25][16];


	void updateBuffer(){
		
	  
		if (getStatus()) {
		  ftdi_read_data(&ftdic, buffer, sizeof(buffer));
		  sortData();
		} else {
			dataBuffer->clear();
		}
	//	lock.exit();
	}



void sortData()
{
    int TTLval, channelVal;

    for (int index = 0; index < sizeof(buffer); index += 3) { 
           
          ++ch; // increment channel number
         // std::cout << ch << std::endl;
            
         thisSample[ch%16] = float((buffer[index] & 127) + 
                     ((buffer[index+1] & 127) << 7) + 
                     ((buffer[index+2] & 3) << 14) - 32768)/32768; 
                     

         TTLval = (buffer[index+2] & 4) >> 2; // extract TTL value (bit 3)
         channelVal = buffer[index+2] & 60;   // extract channel value

         if (channelVal == 60) {

         	//std::cout << *(thisSample + 1) << std::endl;

         	dataBuffer->addToBuffer(thisSample,1);
         	ch = -1; // reset channel to zero
         }

    }
}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DataThread);


};






#endif  // __SOURCENODE_H_DCE798F1__
