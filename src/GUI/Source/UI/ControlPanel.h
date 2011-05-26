/*
  ==============================================================================

    ControlPanel.h
    Created: 1 May 2011 2:57:48pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __CONTROLPANEL_H_AD81E528__
#define __CONTROLPANEL_H_AD81E528__

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Audio/AudioComponent.h"
#include "../Processors/ProcessorGraph.h"

class PlayButton : public DrawableButton
{
	public:
		PlayButton();
		~PlayButton();
};

class RecordButton : public DrawableButton
{
	public:
		RecordButton();
		~RecordButton();
};

class CPUMeter : public Component
{
	public:
		CPUMeter();
		~CPUMeter();

		void paint (Graphics& g);
};

class Clock : public Label
{
	public:
		Clock();
		~Clock();
};



class ControlPanel : public Component, public Button::Listener

{
public:
	ControlPanel(ProcessorGraph* graph, AudioComponent* audio);
	~ControlPanel();

private:	
	PlayButton* playButton;
	RecordButton* recordButton;
	Clock* masterClock;
	CPUMeter* cpuMeter;
	AudioComponent* audio;
	ProcessorGraph* graph;

	void resized();
	void buttonClicked(Button* button);

};


#endif  // __CONTROLPANEL_H_AD81E528__
