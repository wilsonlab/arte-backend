/*
  ==============================================================================

    FilterList.cpp
    Created: 1 May 2011 4:01:50pm
    Author:  jsiegle

  ==============================================================================
*/


#include "FilterList.h"

 FilterList::FilterList()
        : ListBox ("Filter List", 0)
 {
        // tells the ListBox that this object supplies the info about its rows.
      setModel (this);

      setMultipleSelectionEnabled (false);
}

    FilterList::~FilterList()
    {
    }

    //==============================================================================
    // The following methods implement the necessary virtual functions from ListBoxModel,
    // telling the listbox how many rows there are, painting them, etc.
    int FilterList::getNumRows()
    {
        return 20;
    }

    void FilterList::paintListBoxItem (int rowNumber,
                           Graphics& g,
                           int width, int height,
                           bool rowIsSelected)
    {
        if (rowIsSelected)
            g.fillAll (Colours::lightblue);
        else
            g.fillAll (Colours::lightgrey);

        g.setColour (Colours::black);
        g.setFont (height * 0.7f);

        g.drawText ("Filter Number " + String (rowNumber + 1),
                    5, 0, width, height,
                    Justification::centredLeft, true);
    }

    const String FilterList::getDragSourceDescription (const SparseSet<int>& selectedRows)
    {
        // for our drag desctription, we'll just make a list of the selected
        // row numbers - this will be picked up by the drag target and displayed in
        // its box.
        String desc;

        for (int i = 0; i < selectedRows.size(); ++i)
            desc << (selectedRows [i] + 1) << " ";

        return desc.trim();
    }

    //==============================================================================
    // this just fills in the background of the listbox
    void FilterList::paint (Graphics& g)
    {
        g.fillAll (Colours::blue);
    }