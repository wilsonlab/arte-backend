/*
  ==============================================================================

    DataViewport.cpp
    Created: 26 Jan 2012 12:38:09pm
    Author:  jsiegle

  ==============================================================================
*/

#include "DataViewport.h"

DataViewport::DataViewport() :
	TabbedComponent(TabbedButtonBar::TabsAtBottom)
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

 int DataViewport::addTabToDataViewport(String name, Component* component) {

 	if (tabArray.size() == 0)
 		setVisible(true);

     int tabIndex = getTabbedButtonBar().getNumTabs();
     addTab(name, Colours::pink, component, true, tabIndex);
     getTabbedButtonBar().setCurrentTabIndex(tabIndex);

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

 // TabBarButton* DataViewport::createTabButton(const String& tabName, int tabIndex)
 // {
 	
 // 	CustomTabButton* b = new CustomTabButton(tabName, tabIndex);

 // }
