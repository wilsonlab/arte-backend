/*
  ==============================================================================

    UIComponent.h
    Created: 30 Apr 2011 8:33:05pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __UICOMPONENT_H_D97C73CF__
#define __UICOMPONENT_H_D97C73CF__

#include "../../JuceLibraryCode/JuceHeader.h"
#include "ControlPanel.h"
#include "FilterList.h"
#include "FilterViewport.h"
#include "MessageCenter.h"
#include "Configuration.h"
#include "../Processors/DisplayNode.h"
#include "../Processors/ProcessorGraph.h"
#include "../Audio/AudioComponent.h"


class UIComponent : public Component,
				    public DragAndDropContainer,
				    public ActionBroadcaster,
				    public Timer
				    //public Button::Listener

{
public: 
	UIComponent(ProcessorGraph* pgraph, AudioComponent* audio);
	~UIComponent();

	FilterViewport* getViewport() {return filterViewport;}
	Configuration* getConfiguration() {return config;}

private:
	//ControlPanel* controlPanel;
	//Viewport* dataViewport;
	TabbedComponent* dataViewport;
	FilterViewport* filterViewport;
	FilterList* filterList;
	ControlPanel* controlPanel;
	MessageCenter* messageCenter;
	Configuration* config;

	ProcessorGraph* processorGraph;
	AudioComponent* audio;

	Label* infoLabel;
	//Font* Miso;

	void resized();

	void timerCallback();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIComponent);
	
};

/*class BigBlackBox : public Component
{
public: 
	BigBlackBox() {
	
		setBounds(0,0,500,500);
	}
	~BigBlackBox() {}

private:
	
	void paint(Graphics& g) {
	
		g.setColour(Colours::black);
		g.fillAll();
		g.setColour(Colours::yellow);
		for (int n = 0; n < 500; n+=25)
		{
			g.drawLine(0,500,n,n);
		}
		
	}
};*/



#endif  // __UICOMPONENT_H_D97C73CF__
