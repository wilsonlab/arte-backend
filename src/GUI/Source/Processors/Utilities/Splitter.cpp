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
	: GenericProcessor(name_, nSamps, nChans, lock_, id), 
		destNodeA(0), destNodeB(0), activePath(0)
	{
		
	}

Splitter::~Splitter()
{
	
}

AudioProcessorEditor* Splitter::createEditor()
{
	SplitterEditor* editor = new SplitterEditor(this, viewport);
	setEditor(editor);
	
	std::cout << "Creating editor." << std::endl;
	return editor;
}

void Splitter::setDestNode(GenericProcessor* dn)
{

	destNode = dn;

	if (activePath == 0) {
		destNodeA = dn;
	} else {
		destNodeB = dn;
	}
}

void Splitter::switchDest(int destNum) {
	
	activePath = destNum;

	if (destNum == 0) 
	{
		setDestNode(destNodeA);
	} else 
	{
		setDestNode(destNodeB);
	}
}