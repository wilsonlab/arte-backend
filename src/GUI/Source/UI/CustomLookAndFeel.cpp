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
    return 3;
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

    g.setColour(Colour(103,116,140));
   

	if (orientation == TabbedButtonBar::TabsAtRight)
	{
		g.fillRoundedRectangle(5,5,w-5,h-5,5);

		if (isFrontTab)
			g.fillRect(0,5,10,h-5);
    }
	else if (orientation == TabbedButtonBar::TabsAtTop)
	{
		g.fillRoundedRectangle(5,0,w-5,h-5,5);

		if (isFrontTab)
			g.fillRect(5,h-10,w-5,10);
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
	



    //Path tabShape;

    //createTabButtonShape (tabShape, w, h,
     //                     tabIndex, text, button, orientation,
     //                     isMouseOver, isMouseDown, isFrontTab);

    //fillTabButtonShape (g, tabShape, preferredColour,
     //                   tabIndex, text, button, orientation,
    //                    isMouseOver, isMouseDown, isFrontTab);

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

void CustomLookAndFeel::createTabButtonShape (Path& p,
                                        int width, int height,
                                        int /*tabIndex*/,
                                        const String& /*text*/,
                                        Button& /*button*/,
                                        TabbedButtonBar::Orientation orientation,
                                        const bool /*isMouseOver*/,
                                        const bool /*isMouseDown*/,
                                        const bool /*isFrontTab*/)
{
    const float w = (float) width;
    const float h = (float) height;

    float length = w;
    float depth = h;

    if (orientation == TabbedButtonBar::TabsAtLeft
         || orientation == TabbedButtonBar::TabsAtRight)
    {
        swapVariables (length, depth);
    }

    const float indent = (float) getTabButtonOverlap ((int) depth);
    const float overhang = 4.0f;

    if (orientation == TabbedButtonBar::TabsAtLeft)
    {
        p.startNewSubPath (w, 0.0f);
        p.lineTo (0.0f, indent);
        p.lineTo (0.0f, h - indent);
        p.lineTo (w, h);
        p.lineTo (w + overhang, h + overhang);
        p.lineTo (w + overhang, -overhang);
    }
    else if (orientation == TabbedButtonBar::TabsAtRight)
    {
        p.startNewSubPath (0.0f, 0.0f);
        p.lineTo (w, indent);
        p.lineTo (w, h - indent);
        p.lineTo (0.0f, h);
        p.lineTo (-overhang, h + overhang);
        p.lineTo (-overhang, -overhang);
    }
    else if (orientation == TabbedButtonBar::TabsAtBottom)
    {
        p.startNewSubPath (0.0f, 0.0f);
        p.lineTo (indent, h);
        p.lineTo (w - indent, h);
        p.lineTo (w, 0.0f);
        p.lineTo (w + overhang, -overhang);
        p.lineTo (-overhang, -overhang);
    }
    else
    {
        p.startNewSubPath (0.0f, h);
        p.lineTo (indent, 0.0f);
        p.lineTo (w - indent, 0.0f);
        p.lineTo (w, h);
        p.lineTo (w + overhang, h + overhang);
        p.lineTo (-overhang, h + overhang);
    }

    p.closeSubPath();

    p = p.createPathWithRoundedCorners (4.0f);
}

void CustomLookAndFeel::fillTabButtonShape (Graphics& g,
                                      const Path& path,
                                      const Colour& preferredColour,
                                      int /*tabIndex*/,
                                      const String& /*text*/,
                                      Button& button,
                                      TabbedButtonBar::Orientation /*orientation*/,
                                      const bool /*isMouseOver*/,
                                      const bool /*isMouseDown*/,
                                      const bool isFrontTab)
{
    //g.setColour (isFrontTab ? preferredColour
    //                        : preferredColour.withMultipliedAlpha (0.9f));

    g.setColour(Colours::grey.withAlpha(0.8f));

    g.fillPath (path);

    //g.setColour (button.findColour (isFrontTab ? TabbedButtonBar::frontOutlineColourId
     //                                          : TabbedButtonBar::tabOutlineColourId, false)
     //               .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

    //g.strokePath (path, PathStrokeType (isFrontTab ? 1.0f : 0.5f));
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

    Font font (depth * 0.4f);
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
    else
    {
        transform = transform.translated ((float) x, (float) y);
    }

    if (isFrontTab && (button.isColourSpecified (TabbedButtonBar::frontTextColourId) || isColourSpecified (TabbedButtonBar::frontTextColourId)))
        g.setColour (findColour (TabbedButtonBar::frontTextColourId));
    else if (button.isColourSpecified (TabbedButtonBar::tabTextColourId) || isColourSpecified (TabbedButtonBar::tabTextColourId))
        g.setColour (findColour (TabbedButtonBar::tabTextColourId));
    else
        g.setColour (preferredBackgroundColour.contrasting());

   //if (! (isMouseOver || isMouseDown))
   //     g.setOpacity (0.8f);

   // if (! button.isEnabled())
   //     g.setOpacity (0.3f);

    textLayout.draw (g, transform);
}

int CustomLookAndFeel::getTabButtonBestWidth (int /*tabIndex*/,
                                        const String& text,
                                        int tabDepth,
                                        Button&)
{
    Font f (tabDepth * 0.6f);
    return f.getStringWidth (text.trim()) + getTabButtonOverlap (tabDepth) * 2 + 10;
}



void CustomLookAndFeel::drawTabAreaBehindFrontButton (Graphics& g,
                                                int w, int h,
                                                TabbedButtonBar& tabBar,
                                                TabbedButtonBar::Orientation orientation)
{
   
  
/*
    const float shadowSize = 0.2f;

    float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
    Rectangle<int> shadowRect;

    if (orientation == TabbedButtonBar::TabsAtLeft)
    {
        x1 = (float) w;
        x2 = w * (1.0f - shadowSize);
        shadowRect.setBounds ((int) x2, 0, w - (int) x2, h);
    }
    else if (orientation == TabbedButtonBar::TabsAtRight)
    {
        x2 = w * shadowSize;
        shadowRect.setBounds (0, 0, (int) x2, h);
    }
    else if (orientation == TabbedButtonBar::TabsAtBottom)
    {
        y2 = h * shadowSize;
        shadowRect.setBounds (0, 0, w, (int) y2);
    }
    else
    {
        y1 = (float) h;
        y2 = h * (1.0f - shadowSize);
        shadowRect.setBounds (0, (int) y2, w, h - (int) y2);
    }

    g.setGradientFill (ColourGradient (Colours::black.withAlpha (tabBar.isEnabled() ? 0.3f : 0.15f), x1, y1,
                                       Colours::transparentBlack, x2, y2, false));

    shadowRect.expand (2, 2);
    //g.fillRect (shadowRect);

    g.setColour (Colour (0x80000000));

    if (orientation == TabbedButtonBar::TabsAtLeft)
    {
        g.fillRect (w - 1, 0, 1, h);
    }
    else if (orientation == TabbedButtonBar::TabsAtRight)
    {
        g.fillRect (0, 0, 1, h);
    }
    else if (orientation == TabbedButtonBar::TabsAtBottom)
    {
        g.fillRect (0, 0, w, 1);
    }
    else
    {
        g.fillRect (0, h - 1, w, 1);
    }

	*/
}


// create a "+" sign when there are more tabs than there's room to display

Button* CustomLookAndFeel::createTabBarExtrasButton()
{
    const float thickness = 7.0f;
    const float indent = 22.0f;

    Path p;
    p.addEllipse (-10.0f, -10.0f, 120.0f, 120.0f);

    DrawablePath ellipse;
    ellipse.setPath (p);
    ellipse.setFill (Colour (0x99ffffff));

    p.clear();
    p.addEllipse (0.0f, 0.0f, 100.0f, 100.0f);
    p.addRectangle (indent, 50.0f - thickness, 100.0f - indent * 2.0f, thickness * 2.0f);
    p.addRectangle (50.0f - thickness, indent, thickness * 2.0f, 50.0f - indent - thickness);
    p.addRectangle (50.0f - thickness, 50.0f + thickness, thickness * 2.0f, 50.0f - indent - thickness);
    p.setUsingNonZeroWinding (false);

    DrawablePath dp;
    dp.setPath (p);
    dp.setFill (Colour (0x59000000));

    DrawableComposite normalImage;
    normalImage.insertDrawable (ellipse);
    normalImage.insertDrawable (dp);

    dp.setFill (Colour (0xcc000000));

    DrawableComposite overImage;
    overImage.insertDrawable (ellipse);
    overImage.insertDrawable (dp);

    DrawableButton* db = new DrawableButton ("tabs", DrawableButton::ImageFitted);
    db->setImages (&normalImage, &overImage, 0);
    return db;
}