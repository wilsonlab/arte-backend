
//   ==============================================================================

//     SourceNodeFPGA.h
//     Created: 10 May 2011 6:48:56pm
//     Author:  jsiegle

//   ==============================================================================


// #ifndef __SOURCENODEFPGA_H_A4631395__
// #define __SOURCENODEFPGA_H_A4631395__



// #include "../../JuceLibraryCode/JuceHeader.h"
// #include <ftdi.h>
// #include <stdio.h>

// class DataThread;

// class DataBuffer
// {
// public:
// 	DataBuffer(int chans, int size)
// 	 : abstractFifo (size), buffer(chans, size), sampleIndex(0) {
		
// 	}
// 	~DataBuffer() {
		
// 		//deleteAndZero(buffer);

// 	}

// void addToBuffer(float* data, int numItems) {

// 	int startIndex1, blockSize1, startIndex2, blockSize2;
// 	abstractFifo.prepareToWrite(numItems, startIndex1, blockSize1, startIndex2, blockSize2);

// 	//if (blockSize1 > 0) {

// 		//std::cout << startIndex1 << std::endl;

// 	 for (int chan = 0; chan < 16; chan++) {

// 		buffer.copyFrom(chan, // int destChannel
// 						sampleIndex, // int destStartSample
// 						data + chan,  // const float* source
// 						1); // int num samples
// 	 }
// 	//}


// 	//if (blockSize2 > 0) {
// 	//	buffer.copyFrom(chan, startIndex2, data + chan, blockSize2);
// 	//}

// 	sampleIndex++;
	
// 	abstractFifo.finishedWrite(numItems);


// }


// int readAllFromBuffer (AudioSampleBuffer& data, int maxSize)
// {

// 	// check to see if the maximum size is smaller than the total number of available ints
// 	int numItems = (maxSize < abstractFifo.getNumReady()) ? 
// 			maxSize : abstractFifo.getNumReady();
	
// 	int startIndex1, blockSize1, startIndex2, blockSize2;
// 	abstractFifo.prepareToRead(numItems, startIndex1, blockSize1, startIndex2, blockSize2);

// 	//std::cout << buffer.getNumChannels() << std::endl;

// 	//if (blockSize1 > 0) {

// 		for (int chan = 0; chan < data.getNumChannels(); chan++) {
// 			data.copyFrom(chan, // destChan
// 						   0,    // destStartSample
// 						   buffer, // source
// 						   chan,  // sourceChannel
// 						   0,     // sourceStartSample
// 						   numItems); // numSamples
// 		}
// 	//}


// 	//if (blockSize2 > 0)
// 	//	copyData(data + blockSize1, buffer + startIndex2, blockSize2);
	
// 	sampleIndex = 0;

// 	abstractFifo.finishedRead(numItems);

// 	return numItems;

// }


// 	private:
// 		AbstractFifo abstractFifo;
// 		AudioSampleBuffer buffer;

// 		int sampleIndex;

// //void copyIntoBuffer(AudioSampleBuffer* buffer, float* copyFrom, int numItems)
// //{
// //	for (int n = 0; n < numItems; n++)
// //	{

// //		for (int chan = 0; chan < 16; chan++) {
// //			copyTo.copyFrom(chan, startIndex1, data + chan, blockSize1);
// //	 	}
		
// 		//*(copyTo+n) = *(copyFrom+n);
// //
// //}


// };

// class SourceNodeFPGA : public AudioProcessor

// {
// public:
	
// 	// real member functions:
// 	SourceNodeFPGA(const String name, int* nSamples, const CriticalSection& lock);
// 	~SourceNodeFPGA();
	
// 	const String getName() const {return name;}
	
// 	void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock);
// 	void releaseResources();
// 	void processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

// 	void setParameter (int parameterIndex, float newValue);

// 	AudioProcessorEditor* createEditor( ) {return 0;}
// 	bool hasEditor() const {return false;}

// 	// end real member functions
	
// 	// quick implementations of virtual functions, to be changed later:
// 	void reset() {}
// 	void setCurrentProgramStateInformation(const void* data, int sizeInBytes) {}
// 	void setStateInformation(const void* data, int sizeInBytes) {}
// 	void getCurrentProgramStateInformation(MemoryBlock &destData) {}
// 	void getStateInformation (MemoryBlock &destData) {}
// 	void changeProgramName (int index, const String &newName) {}
// 	void setCurrentProgram (int index) {}

// 	const String getInputChannelName (int channelIndex) const {return T(" ");}
// 	const String getOutputChannelName (int channelIndex) const {return T(" ");}
// 	const String getParameterName (int parameterIndex) {return T(" ");}
// 	const String getParameterText (int parameterIndex) {return T(" ");}
// 	const String getProgramName (int index) {return T(" ");}
	
// 	bool isInputChannelStereoPair (int index) const {return true;}
// 	bool isOutputChannelStereoPair (int index) const {return true;}
// 	bool acceptsMidi () const {return false;}
// 	bool producesMidi () const {return false;}
// 	bool isParameterAutomatable(int parameterIndex) {return false;}
// 	bool isMetaParameter(int parameterIndex) {return false;}
	
// 	int getNumParameters() {return 0;}
// 	int getNumPrograms() {return 0;}
// 	int getCurrentProgram() {return 0;}
	
// 	float getParameter (int parameterIndex) {return 1.0;}

	
	
// private:

// 	const String name;
	
// 	DataThread* dataThread;
// 	DataBuffer* inputBuffer;

// 	int* numSamplesInThisBuffer;

// 	const CriticalSection& lock;

// 	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SourceNodeFPGA);

// };


// // DEFINE DATA THREAD:

// class DataThread : public Thread
// {

// public:
// 	DataThread() : Thread ("Data Thread"),
// 			vendorID(0x0403),
// 			productID(0x6010),
// 			baudrate(115200),
// 			startCode(83),
// 			stopCode(115),
// 			isRunning(false),
// 			ch(-1)

// 	{
// 		ftdi_init(&ftdic);
//    		ftdi_set_interface(&ftdic, INTERFACE_ANY);
// 		ftdi_usb_open(&ftdic, vendorID, productID);
// 		ftdi_set_baudrate(&ftdic, baudrate);

// 		std::cout << "FTDI interface initialized." << std::endl;

// 		ftdi_write_data(&ftdic, &startCode, 1);

// 		dataBuffer = new DataBuffer(16, 4096);

// 		startThread();

// 		//thisSample = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// 	}

// 	~DataThread() {

// 		stopThread(500);

// 			ftdi_write_data(&ftdic, &stopCode, 1);
// 			unsigned char buf[4097]; // has to be bigger than the on-chip buffer
// 			ftdi_read_data(&ftdic, buf, sizeof(buf));

// 			ftdi_usb_close(&ftdic);
//     		ftdi_deinit(&ftdic);
// 			std::cout << "FTDI interface destroyed." << std::endl;

// 			delete dataBuffer;
// 			dataBuffer = 0;
// 	}

// 	void run() {

// 		while (! threadShouldExit())
// 		{
// 				const MessageManagerLock mml (Thread::getCurrentThread());

// 				if (! mml.lockWasGained())
// 					return;

// 				updateBuffer();
// 		}

// 	}

// 	DataBuffer* getBufferAddress() {
// 		return dataBuffer;
// 	}

// private:

// 	struct ftdi_context ftdic;
// 	int vendorID, productID;
// 	int baudrate;
	
// 	unsigned char startCode, stopCode;
// 	unsigned char buffer[240]; // should be 5 samples per channel

// 	bool isRunning;
// 	bool startSaving;

// 	DataBuffer* dataBuffer;

// 	short int thisSampleInt16;
// 	float thisSample[16];

// 	int ch;
// 	int accumulator;


// 	//short int data[25][16];


// 	void updateBuffer(){
		
// 		ftdi_read_data(&ftdic, buffer, sizeof(buffer));
// 		sortData();
// 	}



// void sortData()
// {
//     int TTLval, channelVal;

//     for (int index = 0; index < sizeof(buffer); index += 3) { 
           
//           ++ch; // increment channel number
//          // std::cout << ch << std::endl;
            
//          thisSample[ch%16] = float((buffer[index] & 127) + 
//                      ((buffer[index+1] & 127) << 7) + 
//                      ((buffer[index+2] & 3) << 14) - 32768)/32768; 
                     

//          TTLval = (buffer[index+2] & 4) >> 2; // extract TTL value (bit 3)
//          channelVal = buffer[index+2] & 60;   // extract channel value

//          if (channelVal == 60) {

//          	//std::cout << *(thisSample + 1) << std::endl;

//          	dataBuffer->addToBuffer(thisSample,1);
//          	ch = -1; // reset channel to zero
//          }

//     }
// }

// 	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DataThread);


// };




// #endif  // __SOURCENODEFPGA_H_A4631395__
