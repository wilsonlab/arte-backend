/*
  ==============================================================================

    DataViewport.cpp
    Created: 26 Jan 2012 12:38:09pm
    Author:  jsiegle

  ==============================================================================
*/

#include "DataViewport.h"

DataViewport::DataViewport() :
	TabbedComponent(TabbedButtonBar::TabsAtTop)
{

	setTabBarDepth(30);
	setColour(TabbedComponent::outlineColourId,
							Colours::darkgrey);
	setColour(TabbedComponent::backgroundColourId,
							Colours::darkgrey);

}

DataViewport::~DataViewport()
{
	
}