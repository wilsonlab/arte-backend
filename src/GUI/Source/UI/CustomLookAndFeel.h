/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 26 Jan 2012 8:42:00pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __CUSTOMLOOKANDFEEL_H_6B021009__
#define __CUSTOMLOOKANDFEEL_H_6B021009__

#include "../../JuceLibraryCode/JuceHeader.h"


class CustomLookAndFeel : public LookAndFeel

{
public:
	CustomLookAndFeel();
	~CustomLookAndFeel();

	void drawTabButton (Graphics & g, 
						int w, int h, 
						const Colour& preferredColour,
						int tabIndex, const String& text,
						Button& button,
						TabbedButtonBar::Orientation,
						bool isMouseOver,
						bool isMouseDown,
						bool isFrontTab);

	void drawTabButtonText (Graphics& g,
						    int x, int y, int w, int h,
						    const Colour& preferredBackgroundColour,
						    int tabIndex,
						    const String& text,
						    Button& button,
						    TabbedButtonBar::Orientation o,
						    bool isMouseOver,
						    bool isMouseDown,
						    bool isFrontTab);

	int getTabButtonBestWidth (int tabIndex,
								const String& text,
								int tabDepth,
								Button& button);

	int getTabButtonSpaceAroundImage ();

	void drawTabAreaBehindFrontButton (Graphics& g,
									   int w, int h,
									   TabbedButtonBar& tabBar,
									   TabbedButtonBar::Orientation o);

	int getTabButtonOverlap (int tabDepth);

private:	


};


#endif  // __CUSTOMLOOKANDFEEL_H_6B021009__
