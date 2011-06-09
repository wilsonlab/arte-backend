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
        rootItem = new ListItem("Data Sources",true);
        rootItem->setOpen(true);

        addAndMakeVisible (treeView = new TreeView());
        treeView->setRootItem (rootItem);
        treeView->setMultiSelectEnabled(false);
        treeView->setBounds(0,0,200,600);


}

    FilterList::~FilterList()
    {
    }


    //==============================================================================
    // this just fills in the background of the listbox
    void FilterList::paint (Graphics& g)
    {
        g.setColour (Colours::grey.withAlpha(0.5f));
        g.drawRect (0, 0, getWidth(), getHeight(), 2);
    }


    ListItem::ListItem(const String name_, bool containsSubItems_) 
        : name(name_), containsSubItems(containsSubItems_) {

        if (containsSubItems) {
            addSubItem (new ListItem ("Generic Processor 1",false));
            addSubItem (new ListItem ("Generic Processor 2",false));
            addSubItem (new ListItem ("Generic Processor 3",false));
        }

    }
    ListItem::~ListItem() {clearSubItems();}

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

    bool ListItem::mightContainSubItems() {
        return containsSubItems;
    }

    const String ListItem::getUniqueName() {
        return name;
    }

