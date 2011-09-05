/*
  ==============================================================================

    Splitter.cpp
    Created: 17 Aug 2011 1:02:57am
    Author:  jsiegle

  ==============================================================================
*/


#include "Splitter.h"
#include "../Editors/SplitterEditor.h"

Splitter::Splitter(const String name_, int* nSamps, int nChans, const CriticalSection& lock_, int id)
	: GenericProcessor(name_, nSamps, nChans, lock_, id)
	{
		
	}

Splitter::~Splitter()
{
	
}

AudioProcessorEditor* Splitter::createEditor()
{
	SplitterEditor* editor = new SplitterEditor(this, viewport);
	
	std::cout << "Creating editor." << std::endl;
	return editor;
}