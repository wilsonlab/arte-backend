/*
  ==============================================================================

    ControlPanel.cpp
    Created: 1 May 2011 2:57:48pm
    Author:  jsiegle

  ==============================================================================
*/

#include "ControlPanel.h"
#include <stdio.h>


PlayButton::PlayButton()
	: DrawableButton (T("PlayButton"), DrawableButton::ImageFitted)
{

		DrawablePath normal, over, down;

        Path p;
        p.addTriangle (0.0f, 0.0f, 0.0f, 20.0f, 18.0f, 10.0f);
        normal.setPath (p);
        normal.setFill (Colours::lightgrey);
        normal.setStrokeThickness (0.0f);

        over.setPath (p);
        over.setFill (Colours::black);
        over.setStrokeFill (Colours::black);
        over.setStrokeThickness (5.0f);

        setImages (&normal, &over, &over);
        setBackgroundColours(Colours::darkgrey, Colours::yellow);
        setClickingTogglesState (true);
        setTooltip ("Start/stop acquisition");
}

PlayButton::~PlayButton()
{	
}



RecordButton::RecordButton()
	: DrawableButton (T("RecordButton"), DrawableButton::ImageFitted)
{

		DrawablePath normal, over, down;

        Path p;
        p.addEllipse (0.0,0.0,20.0,20.0);
        normal.setPath (p);
        normal.setFill (Colours::lightgrey);
        normal.setStrokeThickness (0.0f);

        over.setPath (p);
        over.setFill (Colours::black);
        over.setStrokeFill (Colours::black);
        over.setStrokeThickness (5.0f);

        setImages (&normal, &over, &over);
        setBackgroundColours(Colours::darkgrey, Colours::red);
        setClickingTogglesState (true);
        setTooltip ("Start/stop writing to disk");
}

RecordButton::~RecordButton()
{	
}


CPUMeter::CPUMeter()
{
}

CPUMeter::~CPUMeter()
{
}

void CPUMeter::paint(Graphics& g)
{
	g.fillAll(Colours::black);
}

Clock::Clock() : Label(T("Clock"),"00:00.00")
{
}

Clock::~Clock()
{
}

ControlPanel::ControlPanel(ProcessorGraph* graph_, AudioComponent* audio_) : 
			graph (graph_), audio(audio_)
{

	playButton = new PlayButton();
	playButton->addListener (this);
	addAndMakeVisible(playButton);

	recordButton = new RecordButton();
	addAndMakeVisible(recordButton);

	masterClock = new Clock();
	addAndMakeVisible(masterClock);

}

ControlPanel::~ControlPanel()
{
	deleteAllChildren();
	graph = 0;
}

void ControlPanel::resized()
{
	int w = getWidth();
	int h = getHeight();

	if (playButton != 0)
		playButton->setBounds(w-h*5,0,h,h);
	
	if (recordButton != 0)
		recordButton->setBounds(w-h*4,0,h,h);

	if (masterClock != 0)
		masterClock->setBounds(w-h*3,0,h*2,h);
}

void ControlPanel::buttonClicked(Button* button) 


{
	//std::cout << "A button was pressed." << std::endl;

	if (button == recordButton)
	{
		std::cout << "Record button pressed." << std::endl;
		if (recordButton->getToggleState())
		{
			playButton->setToggleState(true,true);
			//File file (File::getSpecialLocation (File::userDocumentsDirectory)
			//	.getNonexistentChildFile ("Demo Audio Recording",".wav"));
			//recordNode->startRecording(file);
		} else {
			//if (recordNode->isRecording())
			//	recordNode->stop();

		}

	} else if (button == playButton) {
		std::cout << "Play button pressed." << std::endl;
		if (!playButton->getToggleState())
		{
			recordButton->setToggleState(false,true);
		}

		//playButtonWasPressed = true;

	}

	if (playButton->getToggleState())
	{

		audio->beginCallbacks();

	} else {

		audio->endCallbacks();

	}

}