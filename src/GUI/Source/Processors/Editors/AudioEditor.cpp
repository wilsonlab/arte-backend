/*
  ==============================================================================

    AudioEditor.cpp
    Created: 31 Jul 2011 10:28:36pm
    Author:  jsiegle

  ==============================================================================
*/


#include "AudioEditor.h"

AudioEditor::AudioEditor (AudioNode* owner) 
	: AudioProcessorEditor (owner), isSelected(false),
	  desiredWidth(150)

{
	name = getAudioProcessor()->getName();

	nodeId = owner->getNodeId();

	backgroundColor = Colour(3, 143, 255);

}

AudioEditor::~AudioEditor()
{
	//std::cout << "  Generic editor for " << getName() << " being deleted with " << getNumChildComponents() << " children. " << std::endl;
	deleteAllChildren();
	//delete titleFont;
}

//void GenericEditor::setTabbedComponent(TabbedComponent* tc) {
	
//	tabComponent = tc;

//}

bool AudioEditor::keyPressed (const KeyPress& key)
{
	//std::cout << name << " received " << key.getKeyCode() << std::endl;
}

void AudioEditor::switchSelectedState() 
{
	//std::cout << "Switching selected state" << std::endl;
	isSelected = !isSelected;
	repaint();
}

void AudioEditor::select()
{
	isSelected = true;
	repaint();
	setWantsKeyboardFocus(true);
	grabKeyboardFocus();
}

bool AudioEditor::getSelectionState() {
	return isSelected;
}

void AudioEditor::deselect()
{
	isSelected = false;
	repaint();
	setWantsKeyboardFocus(false);
}

void AudioEditor::paint (Graphics& g)
{

	//g.addTransform(AffineTransform::rotation( double_Pi/20));

	// g.setColour(Colours::black);
	// g.fillRoundedRectangle(0,0,getWidth(),getHeight(),10.0);

	// if (isSelected) {
	g.setColour(backgroundColor);
	// } else {
	// 	g.setColour(Colours::lightgrey);
	// }
	 g.fillRoundedRectangle(1,1,getWidth()-2,getHeight()-2,9.0);

	// g.setColour(Colours::grey);
	// g.fillRoundedRectangle(4,15,getWidth()-8, getHeight()-19,8.0);
	// g.fillRect(4,15,getWidth()-8, 20);

	

	 g.setColour(Colours::black);

	 Font titleFont = Font(14.0, Font::plain);

	// //titleFont.setTypefaceName(T("Miso"));

	 g.setFont(titleFont);
	 g.drawText(name, 8, 10, 100, 7, Justification::left, false);

}
