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
   FilterViewport::FilterViewport(ProcessorGraph* pgraph)
        : message ("Drag-and-drop some rows from the top-left box onto this component!"),
          somethingIsBeingDraggedOver (false), graph(pgraph), lastBound(5), shiftDown(false)
    {

     //    table.setModel (this);
     //    table.setColour (ListBox::outlineColourId, Colours::darkgrey);
     //    table.setOutlineThickness (0);
     //  //  table.getHeader().addColumn("Source",1,100,50,400,TableHeaderComponent::defaultFlags);
     // //   table.getHeader().addColumn("Sink",2,100,50,400,TableHeaderComponent::defaultFlags);

     //  //  table.getHeader().setSortColumnId (1, true);

     //    table.setMultipleSelectionEnabled (false);
     //    table.setBounds(3,3,getWidth()-6,getHeight()-6);
     //    table.setRowHeight (66);
     //    table.getHeader().addListener(this);

     //    addAndMakeVisible (&table);

      addMouseListener(this, true);
      //addKeyListener(this);
     // setWantsKeyboardFocus(true);

    }

    FilterViewport::~FilterViewport()
    {
        //delete(editorArray);
        //editorArray = 0;
    }

    //==============================================================================
    void FilterViewport::paint (Graphics& g)
    {
        //g.fillAll (Colours::orange.withAlpha (0.2f));

       // paintCell (g, 0, 1, 20, 20, 1);
       // table.setBounds(3,3,getWidth()-6,getHeight()-6);

        // draw a red line around the comp if the user's currently dragging something over it..
        if (somethingIsBeingDraggedOver)
        {
            g.setColour (Colours::orange);
            g.drawRect (0, 0, getWidth(), getHeight(), 3);
        } else {
            g.setColour (Colours::grey.withAlpha(0.5f));
            g.drawRect (0, 0, getWidth(), getHeight(), 2);
        }

        //g.setColour (Colours::black);
       // g.setFont (14.0f);
       // g.drawFittedText (message, 10, 0, getWidth() - 20, getHeight(), Justification::centred, 4);

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

        std::cout << "Item dropped." << std::endl;

        const GenericEditor* editor = (const GenericEditor*) graph->createNewProcessor(sourceDescription);

        if (editor != 0) {

            editorArray.add(editor);
            editorArray.getLast()->setViewport(this);
        
         // Component* comp = table.getCellComponent(lastBound,1);
        //comp->addAndMakeVisible(editorArray.getLast());
         //lastBound++;
       // refreshComponentForCell (1, lastBound, true, 0);

            int componentWidth = editorArray.getLast()->desiredWidth;

            addAndMakeVisible(editorArray.getLast());
            editorArray.getLast()->setBounds(lastBound,5,componentWidth,getHeight()-10);

            lastBound+=componentWidth;

        }

        somethingIsBeingDraggedOver = false;
        repaint();
    }

    void FilterViewport::deleteNode (GenericEditor* editor) {

        int indexToDelete = editorArray.indexOf(editor);
        
        graph->removeProcessor(editorArray[indexToDelete]->nodeId);

        //std::cout << "Node ID: " << editorArray[indexToDelete]->nodeId << std::endl;

        editorArray.remove(indexToDelete,true);
            
        lastBound = 5;

        for (int n = 0; n < editorArray.size(); n++)
        {
            int componentWidth = editorArray[n]->desiredWidth;
            editorArray[n]->setBounds(lastBound,5, componentWidth, getHeight()-10);
            lastBound+=componentWidth;
        }

        if (editorArray.size() > 0) {

            if (indexToDelete == editorArray.size()) {
            editorArray[indexToDelete-1]->select();
             } else {
                 editorArray[indexToDelete]->select();
             }
        }

    }

    void FilterViewport::moveSelection (const KeyPress &key) {
        
        if (key.getKeyCode() == key.leftKey) {
            
            for (int i = 0; i < editorArray.size(); i++) {
            
                if (editorArray[i]->getSelectionState() && i > 0) {
                    
                    editorArray[i-1]->select();
                    editorArray[i]->deselect();

                }
               
            }


        } else if (key.getKeyCode() == key.rightKey) {
             
             for (int i = 0; i < editorArray.size()-1; i++) {
            
                if (editorArray[i]->getSelectionState()) {
                    
                    editorArray[i+1]->select();
                    editorArray[i]->deselect();
                    break;

                }
               
            }



        }

    }

    bool FilterViewport::keyPressed (const KeyPress &key) {
        
       // std::cout << key.getKeyCode() << std::endl;
    }

    //void FilterViewport::modifierKeysChanged (const ModifierKeys & modifiers) {
        
   /*     if (modifiers.isShiftDown()) {
            
            std::cout << "Shift key pressed." << std::endl;
            shiftDown  = true;

        } else {


            std::cout << "Shift key released." << std::endl;
            shiftDown = false;
        }*/

    //}

    void FilterViewport::mouseDown(const MouseEvent &e) {
        
      //  std::cout << "Mouse clicked in viewport!" << std::endl;
      //   std::cout << e.getMouseDownX() << std::endl;
       //  std::cout << e.getMouseDownY() << std::endl;

        // const Point<int> p = e.getMouseDownPosition();

        //GenericEditor* c = (GenericEditor*) e.eventComponent;

       // if (!shiftDown) {
        for (int i = 0; i < editorArray.size(); i++) {
            
            if (e.eventComponent == editorArray[i]) {
                editorArray[i]->select();
            } else {
              //  if (!e.mods.isShiftDown())  
                    editorArray[i]->deselect();
            }
        }


        //c->switchSelectedState();

       // std::cout << "Component: " << e.eventComponent << std::endl;

        // // e.eventComponent->switchSelectedState();

        
            //Rectangle<int> bounds = editorArray[i]->getBounds();
            //std::cout << bounds.getX() << " " << bounds.getY() << " "
            //          << bounds.getWidth() << " " << bounds.getHeight() << std::endl;

            //if (bounds.contains(p))
            

    }

    void FilterViewport::mouseDragged(const MouseEvent &e) {
        
       // std::cout << e.getMouseDownX() << " " << e.x << std::endl;

    }