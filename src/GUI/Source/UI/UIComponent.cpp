/*
  ==============================================================================

    UIComponent.cpp
    Created: 30 Apr 2011 8:33:05pm
    Author:  jsiegle

  ==============================================================================
*/

#include "UIComponent.h"
#include <stdio.h>

UIComponent::UIComponent (ProcessorGraph* pgraph, AudioComponent* audio_) 
	: processorGraph(pgraph), audio(audio_)

{	

	infoLabel = new InfoLabel();
	addChildComponent(infoLabel);

	std::cout << "Created info label." << std::endl;

	dataViewport = new DataViewport ();
	addChildComponent(dataViewport);
	dataViewport->addTabToDataViewport("Info",0);
	dataViewport->addTabToDataViewport("Spike Display",0);
	dataViewport->addTabToDataViewport("LFP Display",0);

	std::cout << "Created data viewport." << std::endl;

	filterViewport = new FilterViewport(processorGraph, dataViewport);
	processorGraph->setFilterViewport(filterViewport);
	addAndMakeVisible(filterViewport);

	std::cout << "Created filter viewport." << std::endl;

	controlPanel = new ControlPanel(processorGraph, audio);
	addAndMakeVisible(controlPanel);
	addActionListener(controlPanel);

	std::cout << "Created control panel." << std::endl;

	filterList = new FilterList();
	addAndMakeVisible(filterList);

	std::cout << "Created filter list." << std::endl;

	messageCenter = new MessageCenter();
	processorGraph->addActionListener(messageCenter);
	addAndMakeVisible(messageCenter);

	std::cout << "Created message center." << std::endl;

	config = new Configuration();

	std::cout << "Created configuration object." << std::endl;

	setBounds(0,0,920,700);

	std::cout << "Component width = " << getWidth() << std::endl;
	std::cout << "Component height = " << getHeight() << std::endl;

	std::cout << "Finished UI stuff." << std::endl;
	
}

UIComponent::~UIComponent()
{
	deleteAllChildren();

	deleteAndZero(config);

	processorGraph = 0;
	audio = 0;
}

void UIComponent::resized()
{
	
	int w = getWidth();
	int h = getHeight();

	if (infoLabel != 0)
		infoLabel->setBounds(400,400,400,200);
	
	if (dataViewport != 0) {
		dataViewport->setBounds(212,50,w-230,h-235);
	}
	
	if (filterViewport != 0)
		filterViewport->setBounds(10,h-175,w-20,125);

	if (controlPanel != 0)
		controlPanel->setBounds(10,10,w-20,25);

	if (filterList != 0)
		filterList->setBounds(10,50,192,h-235);

	if (messageCenter != 0)
		messageCenter->setBounds(40,h-40,w-160,30);

}

