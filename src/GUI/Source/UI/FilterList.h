/*
  ==============================================================================

    FilterList.h
    Created: 1 May 2011 4:01:50pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __FILTERLIST_H_1D6290B7__
#define __FILTERLIST_H_1D6290B7__



#include "../../JuceLibraryCode/JuceHeader.h"

//==============================================================================
// this is the listbox containing the draggable source components..

class FilterList : public Component
{
public:
    //==============================================================================
    FilterList();
    ~FilterList();

    void paint (Graphics& g);
private:
   ScopedPointer <TreeView> treeView;
   ScopedPointer <TreeViewItem> rootItem;

};

class ListItem : public TreeViewItem
{
public:
    ListItem(const String, bool);
    ~ListItem();

    void paintItem(Graphics&, int, int);

    bool mightContainSubItems();
    const String getUniqueName();
    const String getDragSourceDescription();

private:
    bool containsSubItems;
    const String name;

};



#endif  // __FILTERLIST_H_1D6290B7__
