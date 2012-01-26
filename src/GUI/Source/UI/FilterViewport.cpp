/*
  ==============================================================================

    FilterViewport.cpp
    Created: 1 May 2011 4:13:45pm
    Author:  jsiegle

  ==============================================================================
*/

#include "FilterViewport.h"

   FilterViewport::FilterViewport(ProcessorGraph* pgraph, DataViewport* tcomp)
        : message ("Drag-and-drop some rows from the top-left box onto this component!"),
          somethingIsBeingDraggedOver (false), graph(pgraph), tabComponent(tcomp), shiftDown(false),
           insertionPoint(0), componentWantsToMove(false), indexOfMovingComponent(-1)
    {

      activeEditor = 0;

      addMouseListener(this, true);

      int numTabs = tabComponent->getTabbedButtonBar().getNumTabs();
      for (int n = 0; n < numTabs; n++) {
          std::cout << "Adding tab." << std::endl;
          tabArray.add(n);
      }

    }

    FilterViewport::~FilterViewport()
    {
        //deleteAllChildren();
        //delete(editorArray);
        //editorArray = 0;
    }

    void FilterViewport::paint (Graphics& g)
    {

        if (somethingIsBeingDraggedOver)
        {
            g.setColour (Colours::magenta);
        } else {
            g.setColour (Colours::grey.withAlpha(0.5f));
        }

        g.fillRoundedRectangle (0, 0, getWidth(), getHeight(), 15);

    }

    int FilterViewport::addTab(String name, Component* component) {

        int tabIndex = tabComponent->getTabbedButtonBar().getNumTabs();
        tabComponent->addTab(name, Colours::pink, component, true, tabIndex);
        tabComponent->getTabbedButtonBar().setCurrentTabIndex(tabIndex);

        tabArray.add(tabIndex);

        return tabIndex;

    }

    void FilterViewport::removeTab(int index) {
        
        int newIndex = tabArray.indexOf(index);
        tabArray.remove(newIndex);

        tabComponent->getTabbedButtonBar().removeTab(newIndex);

    }

    //==============================================================================
    // These methods implement the DragAndDropTarget interface, and allow our component
    // to accept drag-and-drop of objects from other Juce components..

    bool FilterViewport::isInterestedInDragSource (const String& description, Component* component)
    {
        //check the sourceDescription value
        
        if (description.startsWith("Processors")) {
            return false;
        } else {
            return true;
        }
    }

    void FilterViewport::itemDragEnter (const String& /*sourceDescription*/, Component* /*sourceComponent*/, int /*x*/, int /*y*/)
    {
        somethingIsBeingDraggedOver = true;
        repaint();
    }

    void FilterViewport::itemDragMove (const String& /*sourceDescription*/, Component* /*sourceComponent*/, int x, int /*y*/)
    {

       // bool isShifted[editorArray.size()] = false;
        bool foundInsertionPoint = false;

        int lastCenterPoint = 0;
        int leftEdge;
        int centerPoint;

        //std::cout << x << std::endl;

        for (int n = 0; n < editorArray.size(); n++)
        {
            leftEdge = editorArray[n]->getX();
            centerPoint = leftEdge + (editorArray[n]->getWidth())/2;
            
            if (x < centerPoint && x > lastCenterPoint) {
                insertionPoint = n;
                //std::cout << insertionPoint << std::endl;
                foundInsertionPoint = true;
            }

            lastCenterPoint = centerPoint;
        }

        if (!foundInsertionPoint) {
            insertionPoint = editorArray.size();
        }

        refreshEditors();

       // std::cout << insertionPoint << " " << x << " " <<  leftEdge << " " << centerPoint << " " <<  lastCenterPoint << std::endl;

    }

    void FilterViewport::itemDragExit (const String& /*sourceDescription*/, Component* /*sourceComponent*/)
    {
        somethingIsBeingDraggedOver = false;

        repaint();

        refreshEditors();

    }

    void FilterViewport::itemDropped (const String& sourceDescription, Component* /*sourceComponent*/, int /*x*/, int /*y*/)
    {
        message = "last filter dropped: " + sourceDescription;

        std::cout << "Item dropped at insertion point " << insertionPoint << std::endl;

        /// needed to remove const cast --> should be a better way to do this
        String description = sourceDescription.substring(0);

        GenericProcessor* source = 0;
        GenericProcessor* dest = 0;

        //GenericEditor* editor = (GenericEditor*) graph->createNewProcessor(description, this, source, dest);

        if (insertionPoint < editorArray.size()) {
            dest = (GenericProcessor*) editorArray[insertionPoint]->getProcessor();
        }

        if (insertionPoint > 0) {
            source = (GenericProcessor*) editorArray[insertionPoint-1]->getProcessor();
        }

        activeEditor = (GenericEditor*) graph->createNewProcessor(description, this, source, dest);

        std::cout << "Active editor: " << activeEditor << std::endl;

        if (activeEditor != 0) {
            editorArray.insert(insertionPoint,activeEditor);
            addChildComponent(editorArray[insertionPoint]);
        }

        somethingIsBeingDraggedOver = false;
                    
        updateVisibleEditors();
        repaint();
    }

    void FilterViewport::addEditor (GenericEditor* editor) {
        
        std::cout << "Adding editor." << std::endl;

        //somethingIsBeingDraggedOver = true;

        editorArray.add(editor);
        addChildComponent(editorArray.getLast());

        //editor->setVisible(true);
        //childrenChanged();


        updateVisibleEditors();

       // somethingIsBeingDraggedOver = false;
        repaint();

        resized();

      // itemDragEnter ("desc", 0, 5, 5);
      // itemDragExit ("desc", 0);
    //{
     //   somethingIsBeingDraggedOver = true;
     //   repaint();
   // }
        //somethingIsBeingDraggedOver = true;

    }

    void FilterViewport::deleteNode (GenericEditor* editor) {

        int indexToDelete = editorArray.indexOf(editor);
        
        graph->removeProcessor((GenericProcessor*) editorArray[indexToDelete]->getProcessor());

        //std::cout << "Node ID: " << editorArray[indexToDelete]->nodeId << std::endl;

        editorArray.remove(indexToDelete);

        updateVisibleEditors();

        if (editorArray.size() > 0) {

            if (indexToDelete == editorArray.size()) {
            editorArray[indexToDelete-1]->select();
             } else {
                 editorArray[indexToDelete]->select();
             }
        }

        //removeTab(0);

    }

    void FilterViewport::updateVisibleEditors() 
    {

        //std::cout << "Updating " << editorArray.size() << " editors." << std::endl;

        for (int n = 0; n < editorArray.size(); n++)
        {
            editorArray[n]->setVisible(false);
        }

        editorArray.clear();
        
        GenericEditor* editorToAdd = activeEditor;

        while (editorToAdd != 0) 
        {

            editorArray.insert(0,editorToAdd);
            editorToAdd->setVisible(true);

            GenericProcessor* currentProcessor = (GenericProcessor*) editorToAdd->getProcessor();
            GenericProcessor* source = currentProcessor->getSourceNode();

            if (source != 0)
            {
                editorToAdd = (GenericEditor*) source->getEditor();
            } else {
                editorToAdd = 0;
            }
        }

        editorToAdd = activeEditor;

        while (editorToAdd != 0)
        {
            GenericProcessor* currentProcessor = (GenericProcessor*) editorToAdd->getProcessor();
            GenericProcessor* dest = currentProcessor->getDestNode();

            if (dest != 0)
            {
                editorToAdd = (GenericEditor*) dest->getEditor();
                editorArray.add(editorToAdd);
                editorToAdd->setVisible(true);

            } else {
                editorToAdd = 0;
            }
        }

        refreshEditors();
        
    }

    void FilterViewport::refreshEditors () {
        
        int lastBound = 5;

        for (int n = 0; n < editorArray.size(); n++)
        {

            if (somethingIsBeingDraggedOver 
                && n == insertionPoint 
                && (n != indexOfMovingComponent && n != indexOfMovingComponent+1)) {
                lastBound += 10;
            }

            int componentWidth = editorArray[n]->desiredWidth;
            editorArray[n]->setBounds(lastBound,5, componentWidth, getHeight()-10);
            lastBound+=(componentWidth + 10);

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
            
            if (e.eventComponent == editorArray[i]
                 || e.eventComponent->getParentComponent() == editorArray[i]) {
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

    void FilterViewport::mouseDrag(const MouseEvent &e) {
        

        if (editorArray.contains((GenericEditor*) e.originalComponent) && e.y < 15) {


            
            componentWantsToMove = true;
            indexOfMovingComponent = editorArray.indexOf((GenericEditor*) e.originalComponent);

            somethingIsBeingDraggedOver = true;

            bool foundInsertionPoint = false;

            int lastCenterPoint = 0;
            int leftEdge;
            int centerPoint;

            const MouseEvent event = e.getEventRelativeTo(this);
            //std::cout << x << std::endl;

            for (int n = 0; n < editorArray.size(); n++)
            {
                leftEdge = editorArray[n]->getX();
                centerPoint = leftEdge + (editorArray[n]->getWidth())/2;
            
                if (event.x < centerPoint && event.x > lastCenterPoint) {
                    insertionPoint = n;
                    //std::cout << insertionPoint << std::endl;
                    foundInsertionPoint = true;
            }

            lastCenterPoint = centerPoint;
            }

            if (!foundInsertionPoint) {
                insertionPoint = editorArray.size();
            }

            refreshEditors();

            repaint();

            //std::cout << indexOfMovingComponent << " " << insertionPoint << std::endl;


            //std::cout << "Component wants to move." << std::endl;

        }


    }

    void FilterViewport::mouseUp(const MouseEvent &e) {


        if (componentWantsToMove) {
            
            somethingIsBeingDraggedOver = false;
            componentWantsToMove = false;

            //GenericEditor* editor = editorArray.removeAndReturn(indexOfMovingComponent);
            //editorArray.insert
            if (insertionPoint < indexOfMovingComponent)
                editorArray.move(indexOfMovingComponent, insertionPoint);
            else if (insertionPoint > indexOfMovingComponent)
                editorArray.move(indexOfMovingComponent, insertionPoint-1);

            refreshEditors();
            repaint();

        }


    }

    void FilterViewport::mouseExit(const MouseEvent &e) {

        if (componentWantsToMove) {
            
            somethingIsBeingDraggedOver = false;
            componentWantsToMove = false;

            repaint();
            refreshEditors();

        }


    }
    