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
       rootItem = new ListItem("Processors",true);
       rootItem->setOpen(true);

        addAndMakeVisible (treeView = new TreeView());
        treeView->setRootItem (rootItem);
        treeView->setMultiSelectEnabled(false);
        treeView->setBounds(0,0,200,600);
        treeView->setDefaultOpenness(true);
        treeView->setRootItemVisible(false);


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
  //  g.setColour (Colours::grey.withAlpha(0.5f));
   // g.drawRect (0, 0, getWidth(), getHeight(), 2);
}


ListItem::ListItem(const String name_, bool containsSubItems_) 
    : name(name_), containsSubItems(containsSubItems_) {

    if (name.equalsIgnoreCase("Processors")) {
       addSubItem (new ListItem ("Data Sources",true));
       addSubItem (new ListItem ("Filters",true));
       addSubItem (new ListItem ("Visualizers",true));
       addSubItem (new ListItem ("Utilities",true));
    } else if (name.equalsIgnoreCase("Data Sources")) {
       addSubItem (new ListItem ("Intan Demo Board",false));
       addSubItem (new ListItem ("Custom FPGA",false));
       addSubItem (new ListItem ("Network Stream",false));
    } else if (name.equalsIgnoreCase("Filters")) {
       addSubItem (new ListItem ("Bandpass Filter",false));
       addSubItem (new ListItem ("Resampler",false));
       addSubItem (new ListItem ("Thresholder",false));
    }  else if (name.equalsIgnoreCase("Visualizers")) {
       addSubItem (new ListItem ("Stream Viewer",false));
       addSubItem (new ListItem ("Spike Viewer",false));
       addSubItem (new ListItem ("FFT Viewer",false));
    }  else if (name.equalsIgnoreCase("Utilities")) {
       addSubItem (new ListItem ("Splitter",false));
    }

}

ListItem::~ListItem() {}//clearSubItems();}

void ListItem::paintItem(Graphics& g, int width, int height) {
    if (isSelected()) {
        g.fillAll (Colours::lightgrey.withAlpha (0.1f));
    }

    g.setColour (Colours::black);
    g.setFont( height*0.7f);
    g.drawText (getUniqueName(),4, 0, width-4, height, Justification::centredLeft, true);
}

const String ListItem::getDragSourceDescription()
{
    return name;
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

