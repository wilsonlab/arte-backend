/*
  ==============================================================================

    InfoLabel.cpp
    Created: 26 Jan 2012 12:52:07pm
    Author:  jsiegle

  ==============================================================================
*/

#include "InfoLabel.h"

InfoLabel::InfoLabel()// :
	//Label("Info Label", "Open Ephys")
{

	//MemoryInputStream fontStream (BinaryData::misoregular_ttf,
	 //                              BinaryData::misoregular_ttfSize, false);
	//Typeface* typeface = new Typeface (fontStream);

	//miso = new Font (*typeface);
	//miso->setHeight(80.0f);

	//setFont(miso);

	//setColour(Label::textColourId,Colours::lightgrey);

}

InfoLabel::~InfoLabel()
{
	
}

void InfoLabel::paint(Graphics& g)
{
	
	g.setColour(Colour(170,178,183));
	g.fillAll();

}