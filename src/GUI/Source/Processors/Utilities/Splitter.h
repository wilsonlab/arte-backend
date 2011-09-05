/*
  ==============================================================================

    Splitter.h
    Created: 17 Aug 2011 1:02:57am
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __SPLITTER_H_A75239F7__
#define __SPLITTER_H_A75239F7__

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../GenericProcessor.h"

#include <stdio.h>

class Splitter : public GenericProcessor
{
public:

	Splitter(const String name, int* nSamples, int nChans, const CriticalSection& lock, int nodeId);
	~Splitter();

	AudioProcessorEditor* createEditor();

	bool isSplitter() {return true;}
	
};


#endif  // __SPLITTER_H_A75239F7__
