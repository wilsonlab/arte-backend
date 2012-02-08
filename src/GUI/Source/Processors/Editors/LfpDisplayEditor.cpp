/*
  ==============================================================================

    LfpDisplayEditor.cpp
    Created: 8 Feb 2012 12:56:43pm
    Author:  jsiegle

  ==============================================================================
*/


#include "LfpDisplayEditor.h"
#include "../Visualization/LfpDisplayCanvas.h"

SelectorButton::SelectorButton()
	: DrawableButton (T("Selector"), DrawableButton::ImageFitted)
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
        setBackgroundColours(Colours::darkgrey, Colours::green);
        setClickingTogglesState (true);
        setTooltip ("Toggle a state.");

}

SelectorButton::~SelectorButton()
{
}


LfpDisplayEditor::LfpDisplayEditor (GenericProcessor* parentNode, 
									FilterViewport* vp,
									DataViewport* dv) 
	: GenericEditor(parentNode, vp), dataViewport(dv),
	  tabIndex(-1), dataWindow(0),
	  streamBuffer(0), eventBuffer(0)

{
	desiredWidth = 210;

	windowSelector = new SelectorButton();
	windowSelector->addListener(this);
	windowSelector->setBounds(25,25,20,20);
	windowSelector->setToggleState(false,false);
	addAndMakeVisible(windowSelector);

	tabSelector = new SelectorButton();
	tabSelector->addListener(this);
	tabSelector->setBounds(25,50,20,20);
	tabSelector->setToggleState(false,false);
	addAndMakeVisible(tabSelector);

}

LfpDisplayEditor::~LfpDisplayEditor()
{

	if (tabIndex > -1)
	{
		dataViewport->removeTab(tabIndex);
	}

	deleteAllChildren();

}

void LfpDisplayEditor::setBuffers(AudioSampleBuffer* asb, MidiBuffer* mb)
{
	std::cout << "Buffers are set!" << std::endl;
	streamBuffer = asb;
	eventBuffer = mb;


	std::cout << streamBuffer << std::endl;
	std::cout << eventBuffer << std::endl;
}

void LfpDisplayEditor::buttonClicked(Button* button)
{
	if (button == windowSelector)
	{
		if (dataWindow == 0) {
			dataWindow = new DataWindow(windowSelector);

			dataWindow->setContentComponent(new LfpDisplayCanvas(streamBuffer,eventBuffer,getConfiguration()));

			dataWindow->setVisible(true);
			
		} else {
			dataWindow->setVisible(windowSelector->getToggleState());
		}

	} else if (button == tabSelector)
	{
		if (tabSelector->getToggleState() && tabIndex < 0)
		{

			std::cout << "Editor data viewport: " << dataViewport << std::endl;
			tabIndex = dataViewport->addTabToDataViewport("LFP",new LfpDisplayCanvas(streamBuffer,eventBuffer,getConfiguration()));

		} else if (!tabSelector->getToggleState() && tabIndex > -1)
		{
			dataViewport->removeTab(tabIndex);
			tabIndex = -1;
		}
	}
}
