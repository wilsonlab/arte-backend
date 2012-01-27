/*
  ==============================================================================

    CustomLookAndFeel.cpp
    Created: 26 Jan 2012 8:42:01pm
    Author:  jsiegle

  ==============================================================================
*/

#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel() {}

CustomLookAndFeel::~CustomLookAndFeel() {}

//==============================================================================
int CustomLookAndFeel::getTabButtonOverlap (int tabDepth)
{
    return 0; //1 + tabDepth / 4;
}

int CustomLookAndFeel::getTabButtonSpaceAroundImage()
{
    return 6;
}

void CustomLookAndFeel::drawTabButton (Graphics& g,
                                 int w, int h,
                                 const Colour& preferredColour,
                                 int tabIndex,
                                 const String& text,
                                 Button& button,
                                 TabbedButtonBar::Orientation orientation,
                                 const bool isMouseOver,
                                 const bool isMouseDown,
                                 const bool isFrontTab)
{
    int length = w;
    int depth = h;

    if (orientation == TabbedButtonBar::TabsAtLeft
            || orientation == TabbedButtonBar::TabsAtRight)
    {
       swapVariables (length, depth);
    }

    int borderSize = 3; // border between adjacent tabs
    int innerBorderSize = 3;
    int gapSize = 6;     // gap between tab and tabbedComponent
    float cornerSize = 5.0f; // how rounded should the corners be?

    g.setColour(Colour(103,116,140));
   
	if (orientation == TabbedButtonBar::TabsAtRight)
	{
		g.fillRoundedRectangle(5,5,w-5,h-5,5);

		if (isFrontTab)
			g.fillRect(0,5,10,h-5);
    }
	else if (orientation == TabbedButtonBar::TabsAtTop)
	{
		g.fillRoundedRectangle(borderSize,0,w-2*borderSize,h-gapSize,cornerSize-2.0);
		//g.setColour(Colour(170,178,183));
		//g.fillRect(borderSize+innerBorderSize,innerBorderSize
		//             ,w-2*borderSize-2*innerBorderSize,h-gapSize-2*innerBorderSize);

		if (isFrontTab) {
			g.setColour(Colour(103,116,140));
			g.fillRect(borderSize,h-2*gapSize,w-2*borderSize,2*gapSize);
			g.fillRect(0,h-gapSize,w,gapSize);
			
			//g.setColour(Colour(170,178,183));
			//g.fillRect(borderSize+innerBorderSize,innerBorderSize
		     //        ,w-2*borderSize-2*innerBorderSize,h-gapSize-2*innerBorderSize);

			g.setColour(Colour(70,70,75));
			g.fillRoundedRectangle(-borderSize,0,2*borderSize,h,cornerSize);
			g.fillRoundedRectangle(w-borderSize,0,2*borderSize,h,cornerSize);
		}
	}
	else if (orientation == TabbedButtonBar::TabsAtBottom)
	{
		g.fillRoundedRectangle(5,5,w-5,h-5,5);

		if (isFrontTab)
			g.fillRect(5,10,w-5,20);
	}
	else if (orientation == TabbedButtonBar::TabsAtLeft)
	{
		g.fillRoundedRectangle(5,5,w-5,h-5,5);

		if (isFrontTab)
			g.fillRect(5,10,w-5,20);
	}
	
    const int indent = getTabButtonOverlap (depth);
    int x = 0, y = 0;

    if (orientation == TabbedButtonBar::TabsAtLeft
         || orientation == TabbedButtonBar::TabsAtRight)
    {
        y += indent;
        h -= indent * 2;
    }
    else
    {
        x += indent;
        w -= indent * 2;
    }

    drawTabButtonText (g, x, y, w, h, preferredColour,
                       tabIndex, text, button, orientation,
                       isMouseOver, isMouseDown, isFrontTab);
}


void CustomLookAndFeel::drawTabButtonText (Graphics& g,
                                     int x, int y, int w, int h,
                                     const Colour& preferredBackgroundColour,
                                     int /*tabIndex*/,
                                     const String& text,
                                     Button& button,
                                     TabbedButtonBar::Orientation orientation,
                                     const bool isMouseOver,
                                     const bool isMouseDown,
                                     const bool isFrontTab)
{
    int length = w;
    int depth = h;

    if (orientation == TabbedButtonBar::TabsAtLeft
         || orientation == TabbedButtonBar::TabsAtRight)
    {
        swapVariables (length, depth);
    }

    Font font (depth * 0.6f);
    font.setUnderline (button.hasKeyboardFocus (false));

    GlyphArrangement textLayout;
    textLayout.addFittedText (font, text.trim(),
                              0.0f, 0.0f, (float) length, (float) depth,
                              Justification::centred,
                              jmax (1, depth / 12));

    AffineTransform transform;

    if (orientation == TabbedButtonBar::TabsAtLeft)
    {
        transform = transform.rotated (float_Pi * -0.5f)
                             .translated ((float) x, (float) (y + h));
    }
    else if (orientation  == TabbedButtonBar::TabsAtRight)
    {
        transform = transform.rotated (float_Pi * 0.5f)
                             .translated ((float) (x + w), (float) y);
    }
    else if (orientation == TabbedButtonBar::TabsAtTop)
    {
        transform = transform.translated ((float) x, (float) y - 3.0f);
    }
    else
    {
    	transform = transform.translated ((float) x, (float) y + 3.0f);
    }

    if (isFrontTab && (button.isColourSpecified (TabbedButtonBar::frontTextColourId) || isColourSpecified (TabbedButtonBar::frontTextColourId)))
        g.setColour (findColour (TabbedButtonBar::frontTextColourId));
    else if (button.isColourSpecified (TabbedButtonBar::tabTextColourId) || isColourSpecified (TabbedButtonBar::tabTextColourId))
        g.setColour (findColour (TabbedButtonBar::tabTextColourId));
    else
        g.setColour (preferredBackgroundColour.contrasting());

   if (! (isMouseOver || isMouseDown))
        g.setOpacity (0.8f);

    if (! button.isEnabled())
        g.setOpacity (0.3f);

    textLayout.draw (g, transform);
}

int CustomLookAndFeel::getTabButtonBestWidth (int /*tabIndex*/,
                                        const String& text,
                                        int tabDepth,
                                        Button&)
{
    Font f (tabDepth * 0.6f);
    return f.getStringWidth (text.trim()) + getTabButtonOverlap (tabDepth) * 2 + 40;
}



void CustomLookAndFeel::drawTabAreaBehindFrontButton (Graphics& g,
                                                int w, int h,
                                                TabbedButtonBar& tabBar,
                                                TabbedButtonBar::Orientation orientation)
{
   
}

//==================================================================
// SCROLL BAR METHODS : 
//==================================================================

void CustomLookAndFeel::drawScrollbarButton (Graphics& g,
                              ScrollBar& scrollbar,
                              int width, int height,
                              int buttonDirection,
                              bool isScrollBarVertical,
                              bool isMouseOverButton,
                              bool isButtonDown)
{

	 Path p;

	 float w1 = 0.25f;
	 float w2 = 0.75f;

    if (buttonDirection == 0)
        p.addTriangle (width * 0.5f, height * 0.2f,
                       width * w1, height * 0.7f,
                       width * w2, height * 0.7f);
    else if (buttonDirection == 1)
        p.addTriangle (width * 0.8f, height * 0.5f,
                       width * 0.3f, height * w1,
                       width * 0.3f, height * w2);
    else if (buttonDirection == 2)
        p.addTriangle (width * 0.5f, height * 0.8f,
                       width * w1, height * 0.3f,
                       width * w2, height * 0.3f);
    else if (buttonDirection == 3)
        p.addTriangle (width * 0.2f, height * 0.5f,
                       width * 0.7f, height * w1,
                       width * 0.7f, height * w2);

    if (isButtonDown)
        g.setColour (Colours::white);
    else
        g.setColour (Colours::darkgrey);

    g.fillPath (p);

    if (isMouseOverButton)
    	g.strokePath (p, PathStrokeType (1.0f));
	

}

void CustomLookAndFeel::drawScrollbar (Graphics& g,
                        ScrollBar& scrollbar,
                        int x, int y,
                        int width, int height,
                        bool isScrollbarVertical,
                        int thumbStartPosition,
                        int thumbSize,
                        bool isMouseOver,
                        bool isMouseDown)
 
 {

    Path thumbPath;

    const float slotIndent = jmin (width, height) > 15 ? 1.0f : 0.0f;
    const float thumbIndent = slotIndent + 4.0f;
    const float thumbIndentx2 = thumbIndent * 2.0f;

    if (isScrollbarVertical)
    {

        if (thumbSize > 0)
            thumbPath.addRoundedRectangle (x + thumbIndent,
                                           thumbStartPosition + thumbIndent,
                                           width - thumbIndentx2,
                                           thumbSize - thumbIndentx2,
                                           (width - thumbIndentx2) * 0.3f);
    }
    else
    {
       
        if (thumbSize > 0)
            thumbPath.addRoundedRectangle (thumbStartPosition + thumbIndent,
                                           y + thumbIndent,
                                           thumbSize - thumbIndentx2,
                                           height - thumbIndentx2,
                                           (height - thumbIndentx2) * 0.3f);

    }
 
    g.setColour (Colours::darkgrey);
    g.fillPath (thumbPath);

 }
