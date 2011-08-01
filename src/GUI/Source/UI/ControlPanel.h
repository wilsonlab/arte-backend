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
#include "../Processors/Editors/AudioEditor.h"
#include "../Processors/ProcessorGraph.h"
#include "../Processors/RecordNode.h"

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

class CPUMeter : public Label //Component
{
	public:
		CPUMeter();
		~CPUMeter();

		void updateCPU(float usage);

		void paint (Graphics& g);
	
	private:
		float cpu;

};

class DiskSpaceMeter : public Component
{
public:
	DiskSpaceMeter();
	~DiskSpaceMeter();

	void updateDiskSpace(float percent);

	void paint (Graphics& g);

private:
	float diskFree;
	
};

class Clock : public Label
{
	public:
		Clock();
		~Clock();
};



class ControlPanel : public Component, 
					 public Button::Listener,
					 public ActionListener

{
public:
	ControlPanel(ProcessorGraph* graph, AudioComponent* audio);
	~ControlPanel();

private:	
	PlayButton* playButton;
	RecordButton* recordButton;
	Clock* masterClock;
	CPUMeter* cpuMeter;
	DiskSpaceMeter* diskMeter;
	AudioComponent* audio;
	ProcessorGraph* graph;
	AudioEditor* audioEditor;

	void resized();
	void buttonClicked(Button* button);

	void actionListenerCallback(const String& msg);



};


#endif  // __CONTROLPANEL_H_AD81E528__
