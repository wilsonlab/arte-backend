/*
  ==============================================================================

    DataViewport.cpp
    Created: 26 Jan 2012 12:38:09pm
    Author:  jsiegle

  ==============================================================================
*/

#include "DataViewport.h"

DataViewport::DataViewport() :
	TabbedComponent(TabbedButtonBar::TabsAtTop),
	tabDepth(32)
{

	setTabBarDepth(tabDepth);
	setIndent(10); // gap to leave around the edge
				   // of the content component
	setColour(TabbedComponent::outlineColourId,
							Colours::darkgrey);
	setColour(TabbedComponent::backgroundColourId,
							Colours::darkgrey);

}

DataViewport::~DataViewport()
{
	
}

 int DataViewport::addTabToDataViewport(String name, Component* component) {

 	if (tabArray.size() == 0)
 		setVisible(true);

     int tabIndex = getTabbedButtonBar().getNumTabs();
     addTab(name, Colours::lightgrey, component, true, tabIndex);
     getTabbedButtonBar().setCurrentTabIndex(tabIndex);

     setOutline(0);

     tabArray.add(tabIndex);

     return tabIndex;

 }

 void DataViewport::removeTab(int index) {
        
     int newIndex = tabArray.indexOf(index);
     tabArray.remove(newIndex);

     getTabbedButtonBar().removeTab(newIndex);

     if (tabArray.size() == 0)
     	setVisible(false);

 }

void DataViewport::paint(Graphics& g)
{

	const TabbedButtonBar::Orientation o = getOrientation();

	int x = 0;
	int y = 0;
	int r = getWidth();
	int b = getHeight();

	if (o == TabbedButtonBar::TabsAtTop)
        y += tabDepth;
    else if (o == TabbedButtonBar::TabsAtBottom)
        b -= tabDepth;
    else if (o == TabbedButtonBar::TabsAtLeft)
        x += tabDepth;
    else if (o == TabbedButtonBar::TabsAtRight)
        r -= tabDepth;

	g.setColour(Colour(103,116,140));
	g.fillRoundedRectangle(x,y,r-x,b-y,8);

}