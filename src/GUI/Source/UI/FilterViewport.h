/*
  ==============================================================================

    FilterViewport.h
    Created: 1 May 2011 4:13:45pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __FILTERVIEWPORT_H_80260F3F__
#define __FILTERVIEWPORT_H_80260F3F__



#include "../../JuceLibraryCode/JuceHeader.h"


// and this is a component that can have things dropped onto it..

class FilterViewport  : public Component,
                        public DragAndDropTarget
{
public:
    //==============================================================================
    FilterViewport();
    ~FilterViewport();
  

    //==============================================================================
    void paint (Graphics& g);

    bool isInterestedInDragSource (const String& /*sourceDescription*/, Component* /*sourceComponent*/);
    void itemDragEnter (const String& /*sourceDescription*/, Component* /*sourceComponent*/, int /*x*/, int /*y*/);
    void itemDragMove (const String& /*sourceDescription*/, Component* /*sourceComponent*/, int /*x*/, int /*y*/);
    void itemDragExit (const String& /*sourceDescription*/, Component* /*sourceComponent*/);
    void itemDropped (const String& sourceDescription, Component* /*sourceComponent*/, int /*x*/, int /*y*/);


    //=============================================================================

private:
    String message;
    bool somethingIsBeingDraggedOver;
};



#endif  // __FILTERVIEWPORT_H_80260F3F__
