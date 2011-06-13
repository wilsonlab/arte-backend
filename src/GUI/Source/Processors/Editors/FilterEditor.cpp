/*
  ==============================================================================

    FilterEditor.cpp
    Created: 12 Jun 2011 5:04:08pm
    Author:  jsiegle

  ==============================================================================
*/

#include "FilterEditor.h"
#include "../FilterNode.h"
#include <stdio.h>


FilterEditor::FilterEditor (GenericProcessor* parentNode, FilterViewport* vp) 
	: GenericEditor(parentNode, vp), slider(0)

{
	desiredWidth = 250;

	slider = new Slider (T("Slider"));
	slider->setBounds(25,45,200,40);
	slider->setRange(100,5000,100);
	slider->addListener(this);
	addAndMakeVisible(slider);


	//docWindow = new DocumentWindow(T("Spike Window"), Colours::black, DocumentWindow::allButtons);
	//docWindow->centreWithSize(300,200);
	//docWindow->setUsingNativeTitleBar(true);
	//docWindow->setResizable(true,true);
	//docWindow->setVisible(true);

	viewport->addTab("Name",0);

	//tabIndex = 

}

FilterEditor::~FilterEditor()
{
	//std::cout << "    Filter editor for " << getName() << " being deleted with " << getNumChildComponents() << " children. " << std::endl;
	
	//slider = 0;
	//Slider* s = slider.release();
	//delete s;
	//slider = 0;
	//delete slider;
	
	//slider = 0;

	//delete(docWindow);

	deleteAllChildren();
	//slider = 0;	

}

void FilterEditor::sliderValueChanged (Slider* slider)
{

	getAudioProcessor()->setParameter(0,slider->getValue());

	//if (slider->getValue() > 1000) {
	//	tabComponent->addTab(T("Blank"), Colours::lightgrey, 0, true);
	//}

}