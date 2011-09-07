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
	infoLabel = 0;
	dataViewport = 0;
	filterViewport = 0;
	controlPanel = 0;
	filterList = 0;
	messageCenter = 0;
	
	//BorderSize bs = BorderSize(20,20,10,10);

	setBounds(0,0,920,700);

	int w = getWidth();
	int h = getHeight();

	std::cout << "Component width = " << w << std::endl;
	std::cout << "Component height = " << h << std::endl;

	// initialize fonts
	// MemoryInputStream fontStream (BinaryData::misoregular_ttf,
	//                               BinaryData::misoregular_ttfSize, false);
	// Typeface* typeface = new Typeface (fontStream);

	// Font* miso = new Font (*typeface);
	// miso->setHeight(14.0f);

	//Miso = new Font(14.0, Font::plain);
	//Miso->setTypefaceName(T("Miso"));

	// initialize labels
	infoLabel = new Label (T("Info Label"), "arte alpha v0.0");
	//infoLabel->setFont(*miso);

	addAndMakeVisible(infoLabel);
	infoLabel->setColour(Label::textColourId,Colours::lightgrey);
	
	//addAndMakeVisible (tabbedComponent = new TabbedComponent (TabbedButtonBar::TabsAtRight));
	dataViewport = new TabbedComponent (TabbedButtonBar::TabsAtTop);
	dataViewport->setTabBarDepth(30);
	dataViewport->setColour(TabbedComponent::outlineColourId,
							Colours::darkgrey);
	dataViewport->setColour(TabbedComponent::backgroundColourId,
							Colours::darkgrey);
	//dataViewport->addTab
	//addAndMakeVisible(dataViewport);


	// get graph from parent component
	//AudioProcessorGraph::Node* node = processorGraph->getNodeForId(10);
	//AudioProcessor* myProcessor = node->getProcessor();
	//DisplayNode* mp = (DisplayNode*) myProcessor;
	///StreamViewer* streamViewer = mp->createDisplay();
	
	//dataViewport = new Viewport();
	//dataViewport->setViewedComponent(streamViewer);
	//dataViewport->addTab(T("Stream Viewer"), Colours::lightgrey, 0, true);
	

	//std::cout << "Created data viewport." << std::endl;
	//dataViewport->setCurrentTabIndex (0);
	//dataViewport->addTab(T("Blank"), Colours::green, 0, true);
	//dataViewport->addTab(T("Blank"), Colours::blue, 0, true);
	//dataViewport->addTab(T("arte"), Colours::purple, 0, true);
	addAndMakeVisible(dataViewport);

	filterViewport = new FilterViewport(processorGraph, dataViewport);
	addAndMakeVisible(filterViewport);

	std::cout << "Created filter viewport." << std::endl;

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

	messageCenter = new MessageCenter();
	processorGraph->addActionListener(messageCenter);
	addAndMakeVisible(messageCenter);

	//StreamViewer* s
	//addActionListener(streamViewer);
	addActionListener(controlPanel);
	//addActionListener(processorGraph);
	//sendActionMessage(msg);

	config = new Configuration();

	startTimer(15);

	std::cout << "Finished UI stuff." << std::endl;

	//resized();
	
}

UIComponent::~UIComponent()
{
	deleteAllChildren();

	deleteAndZero(config);
	//delete(Miso);
	processorGraph = 0;
	audio = 0;
}

void UIComponent::resized()
{
	
	int w = getWidth();
	int h = getHeight();

	if (infoLabel != 0)
		infoLabel->setBounds(w-95,h-30,100,30);
	
	if (dataViewport != 0) {
		dataViewport->setBounds(240,60,w-260,h-250);
		//Component* vc = dataViewport->getTabContentComponent(dataViewport->getCurrentTabIndex());//getViewedComponent();
		//vc->setBounds(0,0,w-100,h-250);
	}
	
	if (filterViewport != 0)
		filterViewport->setBounds(20,h-175,w-45,125);
		//filterViewport->setBounds(730-h, 140, 120, w-100); //(-500,h-500,200,800);
		//filterViewport->setTransform(AffineTransform::rotation(-double_Pi/2,
    	//       filterViewport->getX(),filterViewport->getY()));
	
	if (controlPanel != 0)
		controlPanel->setBounds(20,10,w-20,25);

	if (filterList != 0)
		filterList->setBounds(20,60,192,h-280);

	if (messageCenter != 0)
		messageCenter->setBounds(20,h-40,w-120,20);

}

void UIComponent::timerCallback() {
	//std::cout << "Message sent." << std::endl;
	sendActionMessage("Refresh");
	//std::cout << "message sent." << std::endl;
}
