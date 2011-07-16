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
	: GenericProcessor(name_, nSamps, nChans, lock_, id)

{
	
}

DisplayNode::~DisplayNode()
{
	//filter = 0;
}

AudioProcessorEditor* DisplayNode::createEditor()
{
	Visualizer* visualizer = new Visualizer(this, viewport);
	
	std::cout << "Creating visualizer." << std::endl;
	return visualizer;

}
