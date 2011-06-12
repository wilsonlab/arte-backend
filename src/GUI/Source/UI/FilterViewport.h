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

// and this is a component that can have things dropped onto it..

class GenericEditor;

class FilterViewport  : public Component,
                        public DragAndDropTarget
                        //public KeyListener
                       // public TableListBoxModel,
                        //public TableHeaderComponent::Listener
{
public:
    //==============================================================================
    FilterViewport(ProcessorGraph* pgraph);
    ~FilterViewport();
  
   /// int getNumRows() { return 1;}
   // void paintRowBackground (Graphics& g, int, int, int, bool rowIsSelected) {

     //   g.fillAll(Colours::darkgrey);
        //if (rowIsSelected)
         //   g.fillAll (Colours::lightblue);
   // }

    //Component* refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected, 
     //                                   Component* existingComponentToUpdate);

   // void paintCell (Graphics& g, int rowNumber, int columnId, int width, int height, bool)
   // {
    //    g.fillAll(Colours::darkgrey);
       // g.setColour (Colours::black.withAlpha(0.2f));
       // g.fillRect (width-1, 0, 1, height);
   // }

    //==============================================================================
    void paint (Graphics& g);

    void mouseDown(const MouseEvent &e);
    void mouseDragged(const MouseEvent &e);
    //void mouseEnter(const MouseEvent &e);
    //void mouseExit
    //void modifierKeysChanged (const ModifierKeys & modifiers);
    bool keyPressed (const KeyPress &key);
    void moveSelection( const KeyPress &key);

    void deleteNode(GenericEditor* editor);


    //void tableColumnsChanged(TableHeaderComponent* tableHeader);
   // void tableColumnsResized(TableHeaderComponent* tableHeader);
   // void tableSortOrderChanged(TableHeaderComponent* tableHeader);

    bool isInterestedInDragSource (const String& /*sourceDescription*/, Component* /*sourceComponent*/);
    void itemDragEnter (const String& /*sourceDescription*/, Component* /*sourceComponent*/, int /*x*/, int /*y*/);
    void itemDragMove (const String& /*sourceDescription*/, Component* /*sourceComponent*/, int /*x*/, int /*y*/);
    void itemDragExit (const String& /*sourceDescription*/, Component* /*sourceComponent*/);
    void itemDropped (const String& sourceDescription, Component* /*sourceComponent*/, int /*x*/, int /*y*/);



    //=============================================================================

private:
    String message;
    bool somethingIsBeingDraggedOver;
    bool shiftDown;
    ProcessorGraph* graph;
    OwnedArray<GenericEditor, CriticalSection> editorArray;

   // TableListBox table;

    float lastBound;

};



#endif  // __FILTERVIEWPORT_H_80260F3F__
