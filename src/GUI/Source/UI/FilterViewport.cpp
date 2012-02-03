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
       insertionPoint(0), componentWantsToMove(false), indexOfMovingComponent(-1), 
       borderSize(6), tabSize(20), tabButtonSize(15), canEdit(true)
{

//   activeEditor = 0;
 // activeTab = 0;

  addMouseListener(this, true);

}

FilterViewport::~FilterViewport()
{
    deleteAllChildren();
}

void FilterViewport::signalChainCanBeEdited(bool t)
{
    canEdit = t;
    if (!canEdit)
        std::cout << "Filter Viewport disabled.";
    else
        std::cout << "Filter Viewport enabled.";

}

void FilterViewport::paint (Graphics& g)
{

    if (somethingIsBeingDraggedOver)
    {
         g.setColour (Colours::magenta);

    } else {
        g.setColour (Colour(127,137,147));
    }

    g.fillRoundedRectangle (tabSize, 0, getWidth(), getHeight(), 8);

    g.setColour (Colour(170,178,183));
    g.fillRect (tabSize+borderSize,borderSize,
                getWidth()-borderSize*2-tabSize,
                getHeight()-borderSize*2);

    if (somethingIsBeingDraggedOver)
    {
        float insertionX = (float) (borderSize) * 2.5 + (float) tabSize;

        int n;
        for (n = 0; n < insertionPoint; n++)
        {
            insertionX += editorArray[n]->getWidth();
            
        }

        if (n > 1)
            insertionX += borderSize*(n-1);

        g.setColour(Colours::orange);
        g.drawLine(insertionX, (float) borderSize,
                   insertionX, (float) getHeight()-(float) borderSize, 3.0f);

    }

}

bool FilterViewport::isInterestedInDragSource (const String& description, Component* component)
{

    if (canEdit && description.startsWith("Processors")) {
        return false;
    } else {
        return true;
    }

}

void FilterViewport::itemDragEnter (const String& /*sourceDescription*/, Component* /*sourceComponent*/, int /*x*/, int /*y*/)
{
    if (canEdit) {
        somethingIsBeingDraggedOver = true;
        repaint();
    }   
}

void FilterViewport::itemDragMove (const String& /*sourceDescription*/, Component* /*sourceComponent*/, int x, int /*y*/)
{

   // bool isShifted[editorArray.size()] = false;
    if (canEdit) {
        bool foundInsertionPoint = false;

        int lastCenterPoint = -1;
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

       // std::cout << insertionPoint << std::endl;

        repaint();
        refreshEditors();
    }

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

    if (canEdit) {

        message = "last filter dropped: " + sourceDescription;

        std::cout << "Item dropped at insertion point " << insertionPoint << std::endl;

        /// needed to remove const cast --> should be a better way to do this
        String description = sourceDescription.substring(0);

      //  GenericProcessor* source = 0;
      //  GenericProcessor* dest = 0;

      //  if (insertionPoint < editorArray.size()) {
       //     dest = (GenericProcessor*) editorArray[insertionPoint]->getProcessor();
       // }

       // if (insertionPoint > 0) {
       //     source = (GenericProcessor*) editorArray[insertionPoint-1]->getProcessor();
       // }

        GenericEditor* activeEditor = (GenericEditor*) graph->createNewProcessor(description);//, source, dest);

        std::cout << "Active editor: " << activeEditor << std::endl;

        if (activeEditor != 0)
        {
            addChildComponent(activeEditor);

           // GenericProcessor* p = (GenericProcessor*) activeEditor->getProcessor();
           
            updateVisibleEditors(activeEditor, 1);

            //editorArray.insert(insertionPoint,activeEditor);   
        }

        somethingIsBeingDraggedOver = false;
                    
      //  updateVisibleEditors();
        repaint();
    }
}

void FilterViewport::addEditor (GenericEditor* editor) {
    
   //  std::cout << "Adding editor." << std::endl;

   //  //somethingIsBeingDraggedOver = true;

   //  editorArray.add(editor);
   //  addChildComponent(editorArray.getLast());

   //  //editor->setVisible(true);
   //  //childrenChanged();


   //  updateVisibleEditors();

   // // somethingIsBeingDraggedOver = false;
   //  repaint();

   //  resized();

  // itemDragEnter ("desc", 0, 5, 5);
  // itemDragExit ("desc", 0);
//{
 //   somethingIsBeingDraggedOver = true;
 //   repaint();
// }
    //somethingIsBeingDraggedOver = true;

}

void FilterViewport::deleteNode (GenericEditor* editor) {

    indexOfMovingComponent = editorArray.indexOf(editor);
   
    updateVisibleEditors(editor, 3);
    
    graph->removeProcessor((GenericProcessor*) editor->getProcessor());

   //  //std::cout << "Node ID: " << editorArray[indexToDelete]->nodeId << std::endl;

   //  editorArray.remove(indexToDelete);

   // // updateVisibleEditors();

   //  if (editorArray.size() > 0) {

   //      if (indexToDelete == editorArray.size()) {
   //      editorArray[indexToDelete-1]->select();
   //       } else {
   //           editorArray[indexToDelete]->select();
   //       }
   //  }

    //removeTab(0);

}

void FilterViewport::createNewTab(GenericEditor* editor)
{
    
    // if (p->getSourceNode() == 0) { // start of a new signal chain
    SignalChainTabButton* t = new SignalChainTabButton();
    t->setEditor(editor);
    
    t->setBounds(0,(tabButtonSize+5)*(signalChainArray.size()),
                 tabButtonSize,tabButtonSize);
    addAndMakeVisible(t);
    signalChainArray.add(t);
    editor->tabNumber(signalChainArray.size()-1);
    t->setToggleState(true,false);

       // }

}

void FilterViewport::removeTab(int tabIndex)
{
    SignalChainTabButton* t = signalChainArray.remove(tabIndex);
    deleteAndZero(t);

    for (int n = 0; n < signalChainArray.size(); n++) 
    {
        signalChainArray[n]->setBounds(0,(tabButtonSize+5)*n,
                 tabButtonSize,tabButtonSize);
        
        int tNum = signalChainArray[n]->getEditor()->tabNumber();
        
        if (tNum > tabIndex)
            signalChainArray[n]->getEditor()->tabNumber(tNum-1);

    }

    // reset numbers
        

}

void FilterViewport::updateVisibleEditors(GenericEditor* activeEditor, int action)

{

   // std::cout << "Updating visible editors." << std::endl;

    // 1 = add
    // 2 = move
    // 3 = remove

    // Step 1: update the editor array
    if (action == 1) /// add
    {
        std::cout << "    Adding editor." << std::endl;
        editorArray.insert(insertionPoint, activeEditor);
    } else if (action == 2) {  /// move
        std::cout << "    Moving editors." << std::endl;
        if (insertionPoint < indexOfMovingComponent)
           editorArray.move(indexOfMovingComponent, insertionPoint);
        else if (insertionPoint > indexOfMovingComponent)
           editorArray.move(indexOfMovingComponent, insertionPoint-1);
    } else if (action == 3) {/// remove
        std::cout << "    Removing editor." << std::endl;

        //GenericProcessor* p = (GenericProcessor*) activeEditor->getProcessor();
       // if (p->getSourceNode() == 0 && p->getDestNode() == 0) {
        //    removeTab(activeEditor->tabNumber());
        //    if (signalChainArray.size() > 0) {
        //        activeEditor = signalChainArray[0].getEditor();
        //    } else {
         //       activeEditor = 0;
         //   }
        //} //else if (p->getSourceNode() == 0 && p->getDestNode() > 0)
        //{
           // activeEditor = 
        //}
        editorArray.remove(indexOfMovingComponent);

        int t = activeEditor->tabNumber();

       // std::cout << "Tab number: " << t << std::endl;

       // std::cout << "Editor array size: " << editorArray.size() << std::endl;

        if (editorArray.size() > 0) // if there are still editors in this chain
        {

            if (t > -1) {// pass on tab
                editorArray[0]->tabNumber(t); 
                signalChainArray[t]->setEditor(editorArray[0]);
            }

            int nextEditor = jmax(0,indexOfMovingComponent-1);
            activeEditor = editorArray[nextEditor];
           // selectEditor(activeEditor);
            //activeEditor->select(); // make the first one active
            
        } else {

            removeTab(t);

            if (signalChainArray.size() > 0) // if there are other chains
            {
                int nextTab = jmax(0,t-1);
                activeEditor = signalChainArray[nextTab]->getEditor(); 
               // activeEditor->select();// make the first chain active
                signalChainArray[nextTab]->setToggleState(true,false); // send it back to update connections   
            } else {
                activeEditor = 0; // nothing is active
            }
        }

    } else { //no change
      //  std::cout << "    Switching tab." << std::endl;
      //  editorArray.clear();
    }

    // Step 2: update connections
    if (action < 4 && editorArray.size() > 0) {

       // std::cout << "      Updating connections..." << std::endl;
            
        GenericProcessor* source = 0;
        GenericProcessor* dest = (GenericProcessor*) editorArray[0]->getProcessor();

        dest->setSourceNode(source); // set first source as 0

        std::cout << "        " << dest->getName() << "::";

        for (int n = 1; n < editorArray.size(); n++)
        {

            dest = (GenericProcessor*) editorArray[n]->getProcessor();
            source = (GenericProcessor*) editorArray[n-1]->getProcessor();

            dest->setSourceNode(source);

            std::cout << dest->getName() << "::";
        }

        dest->setDestNode(0); // set last dest as 0

        std::cout << std::endl;
    }


    // Step 3: check for new tabs

   // int tabNum = -1;

   if (action < 4) {

    //   int maxTab = -1;

        for (int n = 0; n < editorArray.size(); n++)
        {
            GenericProcessor* p = (GenericProcessor*) editorArray[n]->getProcessor();

            if (p->getSourceNode() == 0)// && editorArray[n]->tabNumber() == -1)
            {
                if (editorArray[n]->tabNumber() == -1) 
                {
                    createNewTab(editorArray[n]);
                }
              //  else {
               //     removeTab(editorArray[n]->tabNumber());
               //     editorArray[n]->tabNumber(-1);
               // }
                //tabNum = editorArray[n]->tabNumber();
            } else {
                if (editorArray[n]->tabNumber() > -1) 
                {
                    removeTab(editorArray[n]->tabNumber());
                    //for (int n = 0; n < editorA)
                }

                editorArray[n]->tabNumber(-1); // reset tab status
            }

      //      maxTab = jmax(editorArray[n]->tabNumber(),maxTab);
        }


        // check that tab numbers are correct:
        // int totalTabs = signalChainArray.size() - 1;

        // if (maxTab > totalTabs) 
        // {

        //     for (int n = 0; n < editorArray.size(); n++) 
        //     {
        //         int t = editorArray[n]->tabNumber();

        //         if (t > -1)
        //             editorArray[n]->tabNumber(t-1);       
        //     }
        // }
    }

    //createNewTab(editorArray[newTabIndex]);
    

    // if (editorIsNew) {

    //     GenericProcessor* source = 0;
    //     GenericProcessor* dest = 0;

    //     if (insertionPoint < editorArray.size()) {
    //        dest = (GenericProcessor*) editorArray[insertionPoint]->getProcessor();
    //     }

    //     if (insertionPoint > 0) {
    //        source = (GenericProcessor*) editorArray[insertionPoint-1]->getProcessor();
    //     }

    //     GenericProcessor* p = (GenericProcessor*) activeEditor->getProcessor();

    //     p->setSourceNode(source);
    //     p->setDestNode(dest);

    // }

    // Step 4: Refresh editors in editor array, based on active editor

    for (int n = 0; n < editorArray.size(); n++)
    {
        editorArray[n]->setVisible(false);
    }

    editorArray.clear();

   // if (activeEditor != 0)
    
    GenericEditor* editorToAdd = activeEditor;

    while (editorToAdd != 0) 
    {

        editorArray.insert(0,editorToAdd);
        //editorToAdd->setVisible(true);

        GenericProcessor* currentProcessor = (GenericProcessor*) editorToAdd->getProcessor();
        GenericProcessor* source = currentProcessor->getSourceNode();

        if (source != 0)
        {
            editorToAdd = (GenericEditor*) source->getEditor();

        } else {

           // if (editorToAdd->tabNumber() > -1) {
           //     createNewTab(editorToAdd);
            //    //editorToAdd->tabStatus(true);
           // }

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
            //editorToAdd->setVisible(true);

        } else {
            editorToAdd = 0;
        }
    }

    // Step 5: make sure all editors are visible, and refresh
    for (int n = 0; n < editorArray.size(); n++)
    {
        editorArray[n]->setVisible(true);
    }

    insertionPoint = -1; // make sure all editors are left-justified
    indexOfMovingComponent = -1;
    refreshEditors();
    
}

void FilterViewport::refreshEditors () {
    
    int lastBound = borderSize+tabSize;

    for (int n = 0; n < editorArray.size(); n++)
    {

        if (somethingIsBeingDraggedOver && n == insertionPoint)
        {
            if (indexOfMovingComponent > -1)
            {
                if (n != indexOfMovingComponent && n != indexOfMovingComponent+1)
                {
                   if (n == 0)
                    lastBound += borderSize*3;
                   else
                    lastBound += borderSize*2;
                }
            } else {
                if (n == 0)
                    lastBound += borderSize*3;
                else
                    lastBound += borderSize*2;
            }

        }

        int componentWidth = editorArray[n]->desiredWidth;
        editorArray[n]->setBounds(lastBound, borderSize, componentWidth, getHeight()-borderSize*2);
        //editorArray[n]->setVisible(true);
        //std::cout << editorArray[n]->isVisible() << std::endl;
        lastBound+=(componentWidth + borderSize);

    }

    //repaint();
    //resized();

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

void FilterViewport::selectEditor(GenericEditor* editor)
{  
    for (int i = 0; i < editorArray.size(); i++) {
        
        if (editor == editorArray[i]
             || editor->getParentComponent() == editorArray[i]) {
            editorArray[i]->select();
        } else {
          //  if (!e.mods.isShiftDown())  
                editorArray[i]->deselect();
        }
    } 
}

void FilterViewport::mouseDown(const MouseEvent &e) {
    
    selectEditor((GenericEditor*) e.eventComponent);
  //  std::cout << "Mouse clicked in viewport!" << std::endl;
  //   std::cout << e.getMouseDownX() << std::endl;
   //  std::cout << e.getMouseDownY() << std::endl;

    // const Point<int> p = e.getMouseDownPosition();

    //GenericEditor* c = (GenericEditor*) e.eventComponent;

   // if (!shiftDown) {
  


    //c->switchSelectedState();

   // std::cout << "Component: " << e.eventComponent << std::endl;

    // // e.eventComponent->switchSelectedState();

    
        //Rectangle<int> bounds = editorArray[i]->getBounds();
        //std::cout << bounds.getX() << " " << bounds.getY() << " "
        //          << bounds.getWidth() << " " << bounds.getHeight() << std::endl;

        //if (bounds.contains(p))
        

}

void FilterViewport::mouseDrag(const MouseEvent &e) {
    

    if (editorArray.contains((GenericEditor*) e.originalComponent) 
        && e.y < 15 
        && canEdit
        && editorArray.size() > 1) {

        componentWantsToMove = true;
        indexOfMovingComponent = editorArray.indexOf((GenericEditor*) e.originalComponent);

    }

    if (componentWantsToMove) 
    {

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

          //  if (n == editorArray.size()-1 &&
          //         indexOfMovingComponent == editorArray.size()-1)
          //  {
           ///     centerPoint = leftEdge;
           // }
        
            if (event.x < centerPoint && event.x > lastCenterPoint) 
            {
                
           //     if (n == editorArray.size()-1 &&
           //       indexOfMovingComponent == editorArray.size()-1) 
            //   {
                   // do nothing
             //   } else {
                    insertionPoint = n;
                    //std::cout << insertionPoint << std::endl;
                    foundInsertionPoint = true;
             //   }
            }

            lastCenterPoint = centerPoint;
        }

        



        if (!foundInsertionPoint && indexOfMovingComponent != editorArray.size()-1) {
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


        GenericEditor* editor = editorArray[indexOfMovingComponent];
        //GenericEditor* editor = editorArray.removeAndReturn(indexOfMovingComponent);
        //editorArray.insert
        //if (insertionPoint < indexOfMovingComponent)
         //   editorArray.move(indexOfMovingComponent, insertionPoint);
        //else if (insertionPoint > indexOfMovingComponent)
        //    editorArray.move(indexOfMovingComponent, insertionPoint-1);

       //  for (int n = 1; n < editorArray.size(); n++)
       //  {
       //      GenericProcessor* gp2 = (GenericProcessor*) editorArray[n]->getProcessor();
        //     GenericProcessor* gp1 = (GenericProcessor*) editorArray[n-1]->getProcessor();
             
           //  std::cout << "Processor 1: " << gp1->getName() <<
           //               ", Processor 2: " << gp2->getName() << std::endl;

        //     gp2->setSourceNode(gp1);
             //gp1->setDestNode(gp2);

        // }

        updateVisibleEditors(editor, 2);
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


void SignalChainTabButton::clicked() 
{
    {
    
        //std::cout << "Button clicked: " << firstEditor->getName() << std::endl;
        FilterViewport* fv = (FilterViewport*) getParentComponent();
    
        fv->updateVisibleEditors(firstEditor,4);    
    }
    
}