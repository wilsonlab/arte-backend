/*
  ==============================================================================

    FilterList.cpp
    Created: 1 May 2011 4:01:50pm
    Author:  jsiegle

  ==============================================================================
*/


#include "FilterList.h"

 FilterList::FilterList() : treeView(0)
 {
       rootItem = new ListItem("Processors","none",true);
       rootItem->setOpen(true);

        addAndMakeVisible (treeView = new TreeView());
        treeView->setRootItem (rootItem);
        treeView->setMultiSelectEnabled(false);
        treeView->setBounds(10,10,200,600);
        treeView->setDefaultOpenness(true);
        treeView->setRootItemVisible(false);

        //button = new Button

}

FilterList::~FilterList()
{
    treeView->deleteRootItem();
    deleteAllChildren();

}

//==============================================================================
// this just fills in the background of the listbox
void FilterList::paint (Graphics& g)
{
    g.setColour (Colours::grey.withAlpha(0.5f));
    g.fillRoundedRectangle (0, 0, getWidth(), getHeight(), 12);
    //g.setColour (Colours::black);
   // g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 10, 3);
   // g.setColour (Colours::black);
    //g.drawRoundedRectangle(5, 5, getWidth()-10, getHeight()-10, 8, 2.2);

}


ListItem::ListItem(const String name_, const String parentName_, bool containsSubItems_) 
    : name(name_), parentName(parentName_), containsSubItems(containsSubItems_) {

    if (name.equalsIgnoreCase("Processors")) {
       addSubItem (new ListItem ("Data Sources",name,true));
       addSubItem (new ListItem ("Filters",name,true));
       addSubItem (new ListItem ("Visualizers",name,true));
       addSubItem (new ListItem ("Utilities",name,true));
    } else if (name.equalsIgnoreCase("Data Sources")) {
       addSubItem (new ListItem ("Intan Demo Board",name,false));
       addSubItem (new ListItem ("Custom FPGA",name,false));
       addSubItem (new ListItem ("Network Stream",name,false));
    } else if (name.equalsIgnoreCase("Filters")) {
       addSubItem (new ListItem ("Bandpass Filter",name,false));
       addSubItem (new ListItem ("Resampler",name,false));
       addSubItem (new ListItem ("Thresholder",name,false));
    }  else if (name.equalsIgnoreCase("Visualizers")) {
       addSubItem (new ListItem ("Stream Viewer",name,false));
       addSubItem (new ListItem ("Spike Viewer",name,false));
       addSubItem (new ListItem ("FFT Viewer",name,false));
    }  else if (name.equalsIgnoreCase("Utilities")) {
       addSubItem (new ListItem ("Splitter",name,false));
       addSubItem (new ListItem ("Event Node",name,false));
    }

}

ListItem::~ListItem() {}//clearSubItems();}

void ListItem::paintItem(Graphics& g, int width, int height) {
    
   // if (isSelected()) {
    //    g.fillAll (Colours::lightgrey.withAlpha (0.1f));
   // }

    if (isSelected()) {
        g.setColour (Colours::yellow);
    } else {
        g.setColour (Colours::black);
    }

    g.setFont( height*0.7f);
    g.drawText (getUniqueName(),4, 0, width-4, height, Justification::centredLeft, true);
}

const String ListItem::getDragSourceDescription()
{
    //String parentName = getParentItem()->getUniqueName();
    //std::cout << parentName << std::endl;
    return parentName + "/" + name;
}

// void ListItem::paintOpenCloseButton (Graphics &g, int width, int height, bool isMouseOver)
// {
//     g.setColour(Colours::black);

//     if (isOpen()) {
        
//         g.drawLine(width/4, height/2, width*3/4, height/2, 1.0f);

//     } else {
//         g.drawEllipse(0, 0, height/2, height/2, 1.0f);
//     }
// }

bool ListItem::mightContainSubItems() {
    return containsSubItems;
}

const String ListItem::getUniqueName() {
    return name;
}

