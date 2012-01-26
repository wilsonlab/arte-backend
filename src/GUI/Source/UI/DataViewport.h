/*
  ==============================================================================

    DataViewport.h
    Created: 26 Jan 2012 12:38:09pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __DATAVIEWPORT_H_B38FE628__
#define __DATAVIEWPORT_H_B38FE628__

#include "../../JuceLibraryCode/JuceHeader.h"


class DataViewport : public TabbedComponent

{
public: 
	DataViewport();
	~DataViewport();

    int addTabToDataViewport(String tabName, Component* componentToAdd);
    void removeTab(int);

private:

	//TabBarButton* createTabButton(const String& tabName, int tabIndex);

	Array<int> tabArray;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DataViewport);
	
};

// class CustomTabButton : public TabBarButton

// {
// public:
// 	CustomTabButton();
// 	~CustomTabButton();

// private:	

// };

#endif  // __DATAVIEWPORT_H_B38FE628__
