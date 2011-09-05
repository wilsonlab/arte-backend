/*
  ==============================================================================

    SplitterEditor.cpp
    Created: 5 Sep 2011 2:18:10pm
    Author:  jsiegle

  ==============================================================================
*/

#include "SplitterEditor.h"
#include "../Utilities/Splitter.h"

PipelineSelectorButton::PipelineSelectorButton()
	: DrawableButton (T("Selector"), DrawableButton::ImageFitted)
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
        setBackgroundColours(Colours::darkgrey, Colours::purple);
        setClickingTogglesState (true);
        setTooltip ("Toggle a state.");

}

PipelineSelectorButton::~PipelineSelectorButton()
{
}

SplitterEditor::SplitterEditor (GenericProcessor* parentNode, FilterViewport* vp) 
	: GenericEditor(parentNode, vp)

{
	desiredWidth = 95;

	pipelineSelectorA = new PipelineSelectorButton();
	pipelineSelectorA->addListener(this);
	pipelineSelectorA->setBounds(30,30,30,30);
	pipelineSelectorA->setToggleState(true,false);
	addAndMakeVisible(pipelineSelectorA);

	pipelineSelectorB = new PipelineSelectorButton();
	pipelineSelectorB->addListener(this);
	pipelineSelectorB->setBounds(30,60,30,30);
	pipelineSelectorB->setToggleState(false,false);
	addAndMakeVisible(pipelineSelectorB);

}

SplitterEditor::~SplitterEditor()
{
	deleteAllChildren();
}

void SplitterEditor::buttonClicked(Button* button)
{
	if (button == pipelineSelectorA)
	{
		pipelineSelectorA->setToggleState(true,false);
		pipelineSelectorB->setToggleState(false,false);
		Splitter* processor = (Splitter*) getProcessor();
		processor->switchDest(0);

	} else if (button == pipelineSelectorB) 
	{
		pipelineSelectorB->setToggleState(true,false);
		pipelineSelectorA->setToggleState(false,false);
		Splitter* processor = (Splitter*) getProcessor();
		processor->switchDest(1);
		
	}
}