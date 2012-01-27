/*
  ==============================================================================

    GenericEditor.cpp
    Created: 7 Jun 2011 11:37:12am
    Author:  jsiegle

  ==============================================================================
*/

#include "GenericEditor.h"

GenericEditor::GenericEditor (GenericProcessor* owner, FilterViewport* vp) 
	: AudioProcessorEditor (owner), isSelected(false), viewport(vp),
	  desiredWidth(150)

{
	name = getAudioProcessor()->getName();

	nodeId = owner->getNodeId();

	Random r = Random(99);
	r.setSeedRandomly();

	//titleFont = new Font(14.0, Font::plain);

	//titleFont->setTypefaceName(T("Miso"));

	backgroundColor = Colour(3, 143, 255);

}

GenericEditor::~GenericEditor()
{
	//std::cout << "  Generic editor for " << getName() << " being deleted with " << getNumChildComponents() << " children. " << std::endl;
	deleteAllChildren();
	//delete titleFont;
}

void GenericEditor::setViewport(FilterViewport* vp) {
	
	viewport = vp;

}

//void GenericEditor::setTabbedComponent(TabbedComponent* tc) {
	
//	tabComponent = tc;

//}

bool GenericEditor::keyPressed (const KeyPress& key)
{
	//std::cout << name << " received " << key.getKeyCode() << std::endl;

	if (key.getKeyCode() == key.deleteKey || key.getKeyCode() == key.backspaceKey) {
		
		//std::cout << name << " should be deleted." << std::endl;
		viewport->deleteNode(this);

	} else if (key.getKeyCode() == key.leftKey || key.getKeyCode() == key.rightKey) {

		viewport->moveSelection(key);

	}
}

void GenericEditor::switchSelectedState() 
{
	//std::cout << "Switching selected state" << std::endl;
	isSelected = !isSelected;
	repaint();
}

void GenericEditor::select()
{
	isSelected = true;
	repaint();
	setWantsKeyboardFocus(true);
	grabKeyboardFocus();
}

bool GenericEditor::getSelectionState() {
	return isSelected;
}

void GenericEditor::deselect()
{
	isSelected = false;
	repaint();
	setWantsKeyboardFocus(false);
}

void GenericEditor::paint (Graphics& g)
{
	//g.addTransform(AffineTransform::rotation( double_Pi/20));

	int offset = 5;

	g.setColour(Colour(127,137,147));
	g.fillAll();

	g.setColour(Colours::black);
	g.fillRoundedRectangle(0,0,getWidth()-offset,getHeight(),7.0);

	if (isSelected) {
		g.setColour(backgroundColor);
	} else {
		g.setColour(Colours::lightgrey);
	}
	g.fillRoundedRectangle(1,1,getWidth()-(2+offset),getHeight()-2,6.0);

	g.setColour(Colours::grey);
	g.fillRoundedRectangle(4,15,getWidth()-(8+offset), getHeight()-19,5.0);
	g.fillRect(4,15,getWidth()-(8+offset), 20);

	g.setColour(Colours::black);

	Font titleFont = Font(14.0, Font::plain);

	//titleFont.setTypefaceName(T("Miso"));

	g.setFont(titleFont);
	g.drawText(name, 8, 4, 100, 7, Justification::left, false);

}
