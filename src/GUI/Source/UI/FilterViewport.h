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
class SignalChainTabButton;

class FilterViewport  : public Component,
                        public DragAndDropTarget//,
                        //public KeyListener
{
public:

    FilterViewport(ProcessorGraph* pgraph, DataViewport* tabComp);
    ~FilterViewport();

    void paint (Graphics& g);

    // Creating and deleting editors:
    void deleteNode(GenericEditor* editor);
    void addEditor (GenericEditor*);
    void updateVisibleEditors(GenericEditor* activeEditor, int action);
    void selectEditor(GenericEditor* e);
   // void setActiveEditor(GenericEditor* e) {activeEditor = e; updateVisibleEditors();}

    void signalChainCanBeEdited(bool t);

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

  //  void updateActiveEditor(GenericEditor* e)
  //  {
  //      activeEditor = e;
   //     updateVisibleEditors();
   // }
    
private:

    String message;
    bool somethingIsBeingDraggedOver;
    bool shiftDown;

    bool canEdit;

    ProcessorGraph* graph;
    DataViewport* tabComponent;

    Array<GenericEditor*, CriticalSection> editorArray;
    Array<SignalChainTabButton*, CriticalSection> signalChainArray;
 //   GenericEditor* activeEditor;

   // int activeTab;

    void refreshEditors();
    void createNewTab(GenericEditor* editor);
    void removeTab(int tabIndex);
    //void drawTabs();

    int borderSize, tabSize, tabButtonSize;

    int insertionPoint;
    bool componentWantsToMove;
    int indexOfMovingComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterViewport);  

};

class SignalChainTabButton : public Button
{
public:
    SignalChainTabButton() : Button("Name") 
    {
        setRadioGroupId(99);
        //setToggleState(false,true);
        setClickingTogglesState(true);
    }
    ~SignalChainTabButton() {}

    void setEditor(GenericEditor* p) {firstEditor = p;}
    GenericEditor* getEditor() {return firstEditor;}

private:

    GenericEditor* firstEditor;

    void paintButton(Graphics &g, bool isMouseOver, bool isButtonDown)
    {
        if (getToggleState() == true)
            g.setColour(Colours::teal);
        else 
            g.setColour(Colours::darkgrey);

        if (isMouseOver)
            g.setColour(Colours::white);
        
        //if (isButtonDown)
        //    g.setColour(Colours::black);
       // else if (isOver() == ButtonState::buttonOver)
        //    g.setColour(Colours::white);
       // else 
       //     g.setColour(Colours::darkgrey);

        g.fillEllipse(0,0,getWidth(),getHeight());
    }

    void clicked();


};

#endif  // __FILTERVIEWPORT_H_80260F3F__
