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
	desiredWidth = 275;

	timebaseSlider = new Slider (T("Time Base Slider"));
	timebaseSlider->setBounds(60,20,200,40);
	timebaseSlider->setRange(500,10000,500);
	timebaseSlider->addListener(this);
	addAndMakeVisible(timebaseSlider);

	displayGainSlider = new Slider (T("Display Gain Slider"));
	displayGainSlider->setBounds(60,65,200,40);
	displayGainSlider->setRange(1,8,1);
	displayGainSlider->addListener(this);
	addAndMakeVisible(displayGainSlider);

	windowSelector = new SelectorButton();
	windowSelector->addListener(this);
	windowSelector->setBounds(25,25,20,20);
	windowSelector->setToggleState(false,false);
	addAndMakeVisible(windowSelector);

	tabSelector = new SelectorButton();
	tabSelector->addListener(this);
	tabSelector->setBounds(25,50,20,20);
	
	addAndMakeVisible(tabSelector);
	tabSelector->setToggleState(false,false);

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

			//dataWindow->setContentComponent(new LfpDisplayCanvas(streamBuffer,eventBuffer,getConfiguration(), this));

			if (tabSelector->getToggleState())
			{
				tabSelector->setToggleState(false, false);
				dataViewport->removeTab(tabIndex);
				tabIndex = -1;
			}

			//Component* p = (Component*) getProcessor();

			dataWindow->setContentNonOwned((LfpDisplayNode*) getProcessor(), false);
			//getProcessor()->parentComponentChanged();
			dataWindow->setVisible(true);
			
		} else {

			if (tabSelector->getToggleState())
			{
				tabSelector->setToggleState(false, false);
				dataViewport->removeTab(tabIndex);
				tabIndex = -1;
			}

			dataWindow->setVisible(windowSelector->getToggleState());
			//getProcessor()->parentComponentChanged();
		}

	} else if (button == tabSelector)
	{
		if (tabSelector->getToggleState() && tabIndex < 0)
		{

			//std::cout << "Editor data viewport: " << dataViewport << std::endl;
			
			if (windowSelector->getToggleState())
			{
				windowSelector->setToggleState(false, false);
				dataWindow->setVisible(false);
			}

			//tabIndex = dataViewport->addTabToDataViewport("LFP",new LfpDisplayCanvas(streamBuffer,eventBuffer,getConfiguration(), this));
			//Component* p = (Component*) getProcessor();


			tabIndex = dataViewport->addTabToDataViewport("LFP",(LfpDisplayNode*) getProcessor());

		} else if (!tabSelector->getToggleState() && tabIndex > -1)
		{
			dataViewport->removeTab(tabIndex);
			tabIndex = -1;
		}
	}
}

void LfpDisplayEditor::sliderValueChanged (Slider* slider)
{

	if (slider == timebaseSlider)
		getAudioProcessor()->setParameter(0,slider->getValue());
	//else 
	//	getAudioProcessor()->setParameter(1,slider->getValue());

}