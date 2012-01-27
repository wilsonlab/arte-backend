/*
  ==============================================================================

    InfoLabel.h
    Created: 26 Jan 2012 12:52:07pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __INFOLABEL_H_14DA9A62__
#define __INFOLABEL_H_14DA9A62__

#include "../../JuceLibraryCode/JuceHeader.h"

class InfoLabel : public Component

{
public: 
	InfoLabel();
	~InfoLabel();

private:

	//Font* miso;
	void paint(Graphics& g);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoLabel);
	
};



#endif  // __INFOLABEL_H_14DA9A62__
