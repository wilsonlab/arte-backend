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
#include "../Processors/ProcessorGraph.h"
#include "../Processors/Editors/GenericEditor.h"
#include "DataViewport.h"

class GenericEditor;

class FilterViewport  : public Component,
                        public DragAndDropTarget
                        //public KeyListener
{
public:

    FilterViewport(ProcessorGraph* pgraph, DataViewport* tabComp);
    ~FilterViewport();

    void paint (Graphics& g);

    // Creating and deleting editors:
    void deleteNode(GenericEditor* editor);
    void addEditor (GenericEditor*);
    void updateVisibleEditors();
    void setActiveEditor(GenericEditor* e) {activeEditor = e; updateVisibleEditors();}

    // Interactions with tabbed component:
    int addTab(String tabName, Component* componentToAdd);
    void removeTab(int);

    // DragAndDropTarget methods:
    bool isInterestedInDragSource (const String& /*sourceDescription*/, Component* /*sourceComponent*/);
    void itemDragEnter (const String& /*sourceDescription*/, Component* /*sourceComponent*/, int /*x*/, int /*y*/);
    void itemDragMove (const String& /*sourceDescription*/, Component* /*sourceComponent*/, int /*x*/, int /*y*/);
    void itemDragExit (const String& /*sourceDescription*/, Component* /*sourceComponent*/);
    void itemDropped (const String& sourceDescription, Component* /*sourceComponent*/, int /*x*/, int /*y*/);

    // mouse and keypress methods:
    void mouseDown(const MouseEvent &e);
    void mouseDrag(const MouseEvent &e);
    void mouseUp(const MouseEvent &e);
    void mouseExit(const MouseEvent &e);
    //void mouseEnter(const MouseEvent &e);
    //void mouseExit
    //void modifierKeysChanged (const ModifierKeys & modifiers);
    bool keyPressed (const KeyPress &key);
    void moveSelection( const KeyPress &key);
    
private:
    String message;
    bool somethingIsBeingDraggedOver;
    bool shiftDown;

    ProcessorGraph* graph;
    DataViewport* tabComponent;

    Array<GenericEditor*, CriticalSection> editorArray;
    GenericEditor* activeEditor;
    Array<int> tabArray;

    void refreshEditors();

    int insertionPoint;
    bool componentWantsToMove;
    int indexOfMovingComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterViewport);  

};



#endif  // __FILTERVIEWPORT_H_80260F3F__
