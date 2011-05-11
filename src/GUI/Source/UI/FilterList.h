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

class FilterList   : public ListBox,
                     public ListBoxModel
{
public:
    //==============================================================================
    FilterList();
    ~FilterList();
    int getNumRows();

    void paintListBoxItem (int rowNumber,
                           Graphics& g,
                           int width, int height,
                           bool rowIsSelected);

    const String getDragSourceDescription (const SparseSet<int>& selectedRows);
    void paint (Graphics& g);
};



#endif  // __FILTERLIST_H_1D6290B7__
