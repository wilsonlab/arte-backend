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



class ControlPanel : public Component

{
public:
	ControlPanel();
	~ControlPanel();

private:	
	PlayButton* playButton;
	RecordButton* recordButton;
	Clock* masterClock;
	CPUMeter* cpuMeter;

	void resized();

};


#endif  // __CONTROLPANEL_H_AD81E528__
