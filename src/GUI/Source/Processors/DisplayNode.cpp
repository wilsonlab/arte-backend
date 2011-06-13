/*
  ==============================================================================

    DisplayNode.cpp
    Created: 7 May 2011 5:07:43pm
    Author:  jsiegle

  ==============================================================================
*/


#include "DisplayNode.h"
#include <stdio.h>

DisplayNode::DisplayNode(const String name_, int* nSamps, int nChans, const CriticalSection& lock_, int id)
	: ResamplingNode(name_, nSamps, nChans, lock_, id, false)

{
	
}

DisplayNode::~DisplayNode()
{
	//filter = 0;
}

AudioProcessorEditor* DisplayNode::createEditor()
{
	StreamViewer* streamViewer = new StreamViewer(this, viewport);
	
	std::cout << "Creating Stream Viewer." << std::endl;
	return streamViewer;

}
