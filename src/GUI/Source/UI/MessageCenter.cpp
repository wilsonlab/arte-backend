/*
  ==============================================================================

    MessageCenter.cpp
    Created: 31 Jul 2011 3:31:01pm
    Author:  jsiegle

  ==============================================================================
*/

#include "MessageCenter.h"


MessageCenter::MessageCenter() : 
	messageBackground(Colours::grey.withAlpha(0.5f)) {
	
	messageDisplayArea = new Label("Message Display Area","No new messages.");

	addAndMakeVisible(messageDisplayArea);

}

MessageCenter::~MessageCenter() {
	

	deleteAllChildren();

}

void MessageCenter::paint(Graphics& g)
{
	
	g.setColour (Colour(50,50,50));

	g.fillRoundedRectangle (0, 0, getWidth(), getHeight(), 5);

	g.setColour (messageBackground);
   
   	g.fillRoundedRectangle (5, 5, getWidth()-10, getHeight()-10, 3);

}

void MessageCenter::resized() 
{
	if (messageDisplayArea != 0)
		messageDisplayArea->setBounds(5,0,getWidth(),getHeight());

}

void MessageCenter::actionListenerCallback(const String& message)
{
	
	messageDisplayArea->setText(message,false);

	messageBackground = Colours::yellow;

	repaint();

}