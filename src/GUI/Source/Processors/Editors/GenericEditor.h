/*
  ==============================================================================

    GenericEditor.h
    Created: 7 Jun 2011 11:37:12am
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __GENERICEDITOR_H_DD406E71__
#define __GENERICEDITOR_H_DD406E71__


#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../GenericProcessor.h"
#include "../../UI/FilterViewport.h"
#include <stdio.h>

class GenericProcessor;
class FilterViewport;

class GenericEditor : public AudioProcessorEditor

{
public:
	GenericEditor (GenericProcessor* owner, FilterViewport* vp);
	virtual ~GenericEditor();

	void paint (Graphics& g);
	void setViewport(FilterViewport*);
	//void setTabbedComponent(TabbedComponent*);

	bool keyPressed (const KeyPress& key);

	void switchSelectedState();
	void select();
	void deselect();
	bool getSelectionState();

	String getName() {return name;}

	//TabbedComponent* tabComponent;


	int desiredWidth;
	int nodeId;

	FilterViewport* viewport;
	
private:

	AudioProcessor* getProcessor() const {

		return getAudioProcessor();
		//return static_cast <GenericProcessor*> (getAudioProcessor());
	}	

	Colour backgroundColor;



	//Font* titleFont;


	bool isSelected;
	String name;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GenericEditor);

};




#endif  // __GENERICEDITOR_H_DD406E71__
