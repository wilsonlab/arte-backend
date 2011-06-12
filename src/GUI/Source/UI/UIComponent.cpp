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
	: processorGraph(pgraph), audio(audio_), msg(T("Message string"))

{
	infoLabel = 0;
	dataViewport = 0;
	filterViewport = 0;
	controlPanel = 0;
	filterList = 0;
	
	//BorderSize bs = BorderSize(20,20,10,10);

	setBounds(0,0,800,600);

	int w = getWidth();
	int h = getHeight();

	std::cout << "Component width = " << w << std::endl;
	std::cout << "Component height = " << h << std::endl;

	// initialize fonts
	Miso = new Font(14.0, Font::plain);
	Miso->setTypefaceName(T("Miso"));

	// initialize labels
	infoLabel = new Label (T("Info Label"), "arte alpha v0.0");
	addAndMakeVisible(infoLabel);
	infoLabel->setFont(*Miso);
	infoLabel->setColour(Label::textColourId,Colours::lightgrey);
	
	// get graph from parent component
	AudioProcessorGraph::Node* node = processorGraph->getNodeForId(10);
	AudioProcessor* myProcessor = node->getProcessor();
	DisplayNode* mp = (DisplayNode*) myProcessor;
	StreamViewer* streamViewer = mp->createDisplay();
	
	dataViewport = new Viewport();
	dataViewport->setViewedComponent(streamViewer);
	addAndMakeVisible(dataViewport);
	
	filterViewport = new FilterViewport(processorGraph);
	addAndMakeVisible(filterViewport);

	//AffineTransform at = AffineTransform();
   // 	at.rotated(1.5708f);

  // filterViewport->setBounds(150,200,200,500);

  // Point<int> p = getLocalPoint(filterViewport, filterViewport->getPosition());
  //  	filterViewport->setTransform(AffineTransform::rotation(-double_Pi/2,
   // 	       float(p.getX()), float(p.getY())));
    	       //filterViewport->getX(),filterViewport->getY()));
	
	controlPanel = new ControlPanel(processorGraph, audio);
	addAndMakeVisible(controlPanel);

	filterList = new FilterList();
	addAndMakeVisible(filterList);

	
	//StreamViewer* s
	addActionListener(streamViewer);
	addActionListener(controlPanel);
	//sendActionMessage(msg);

	startTimer(15);

	//resized();
	
}

UIComponent::~UIComponent()
{
	deleteAllChildren();
	delete(Miso);
	processorGraph = 0;
	audio = 0;
}

void UIComponent::resized()
{
	
	int w = getWidth();
	int h = getHeight();

	if (infoLabel != 0)
		infoLabel->setBounds(w-80,h-30,100,30);
	
	if (dataViewport != 0) {
		dataViewport->setBounds(240,60,w-100,h-250);
		Component* vc = dataViewport->getViewedComponent();
		vc->setBounds(0,0,w-140,h-250);
	}
	
	if (filterViewport != 0)
		filterViewport->setBounds(25,h-150,w-50,125);
		//filterViewport->setBounds(730-h, 140, 120, w-100); //(-500,h-500,200,800);
		//filterViewport->setTransform(AffineTransform::rotation(-double_Pi/2,
    	//       filterViewport->getX(),filterViewport->getY()));
	
	if (controlPanel != 0)
		controlPanel->setBounds(20,10,w-20,25);

	if (filterList != 0)
		filterList->setBounds(20,60,192,h-300);

}

void UIComponent::timerCallback() {
	//std::cout << "Message sent." << std::endl;
	sendActionMessage(msg);
	//std::cout << "message sent." << std::endl;
}
