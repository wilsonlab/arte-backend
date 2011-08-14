/*
  ==============================================================================

    FileReader.cpp
    Created: 13 Aug 2011 7:18:22pm
    Author:  jsiegle

  ==============================================================================
*/

#include "FileReader.h"


FileReader::FileReader(const String name_, int* nSamps, int nChans, const CriticalSection& lock_, int id)
	: GenericProcessor(name_, nSamps, nChans, lock_, id),
	  sampleRate (40000.0),
	  numChannels(16),
	  samplesPerBlock(1024)
	
{

	setNumOutputs(numChannels);
	setNumInputs(0);

}

FileReader::~FileReader()
{
}


//AudioProcessorEditor* FileReader::createEditor( )
//{
	//filterEditor = new FilterEditor(this);
	
//	std::cout << "Creating editor." << std::endl;
//	sourceEditor = new SourceNodeEditor(this);
//	return sourceEditor;
//}

//AudioProcessorEditor* FilterNode::createEditor(AudioProcessorEditor* const editor)
//{
	
//	return editor;
//}
void FileReader::setParameter (int parameterIndex, float newValue)
{
	//std::cout << "Message received." << std::endl;


}

void FileReader::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{

	samplesPerBlock = int(float(estimatedSamplesPerBlock)/sampleRate*sampleRate);	

	std::cout << "Samples per block = " << samplesPerBlock << std::endl;

	setNumSamples(samplesPerBlock);
}

void FileReader::enable () {

	File file = File("./data_stream_16ch");
	input = file.createInputStream();

	
	std::cout << "File Reader received enable signal." << std::endl;

}

void FileReader::disable() {

	deleteAndZero(input);
	
	std::cout << "File reader received disable signal." << std::endl;

}

void FileReader::releaseResources() 
{	

}

void FileReader::processBlock (AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{

	//std::cout << buffer.getNumChannels() << std::endl;
	
    for (int i = 0; i < samplesPerBlock; ++i)
    {

    	for (int j = 0; j < numChannels; j++) {
    		
    		if (input->isExhausted())
    			input->setPosition(0);   			
    	
    		const float sample = float(input->readShort());

    		*buffer.getSampleData (j, i) = sample;
    	}

    }
}