/*
  ==============================================================================

    FilterViewport.cpp
    Created: 1 May 2011 4:13:45pm
    Author:  jsiegle

  ==============================================================================
*/

// and this is a component that can have things dropped onto it..

#include "FilterViewport.h"


    //==============================================================================
   FilterViewport::FilterViewport()
        : message ("Drag-and-drop some rows from the top-left box onto this component!\n\n"
                   "You can also drag-and-drop files here"),
          somethingIsBeingDraggedOver (false)
    {
    }

    FilterViewport::~FilterViewport()
    {
    }

    //==============================================================================
    void FilterViewport::paint (Graphics& g)
    {
        g.fillAll (Colours::orange.withAlpha (0.2f));

        // draw a red line around the comp if the user's currently dragging something over it..
        if (somethingIsBeingDraggedOver)
        {
            g.setColour (Colours::orange);
            g.drawRect (0, 0, getWidth(), getHeight(), 3);
        }

        g.setColour (Colours::black);
        g.setFont (14.0f);
        g.drawFittedText (message, 10, 0, getWidth() - 20, getHeight(), Justification::centred, 4);
    }

    //==============================================================================
    // These methods implement the DragAndDropTarget interface, and allow our component
    // to accept drag-and-drop of objects from other Juce components..

    bool FilterViewport::isInterestedInDragSource (const String& /*sourceDescription*/, Component* /*sourceComponent*/)
    {
        // normally you'd check the sourceDescription value to see if it's the
        // sort of object that you're interested in before returning true, but for
        // the demo, we'll say yes to anything..
        return true;
    }

    void FilterViewport::itemDragEnter (const String& /*sourceDescription*/, Component* /*sourceComponent*/, int /*x*/, int /*y*/)
    {
        somethingIsBeingDraggedOver = true;
        repaint();
    }

    void FilterViewport::itemDragMove (const String& /*sourceDescription*/, Component* /*sourceComponent*/, int /*x*/, int /*y*/)
    {
    }

    void FilterViewport::itemDragExit (const String& /*sourceDescription*/, Component* /*sourceComponent*/)
    {
        somethingIsBeingDraggedOver = false;
        repaint();
    }

    void FilterViewport::itemDropped (const String& sourceDescription, Component* /*sourceComponent*/, int /*x*/, int /*y*/)
    {
        message = "last filter dropped: " + sourceDescription;

        somethingIsBeingDraggedOver = false;
        repaint();
    }