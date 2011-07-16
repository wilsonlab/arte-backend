/*
  ==============================================================================

    StreamViewer.h
    Created: 1 May 2011 4:48:59pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __STREAMVIEWER_H_BC589573__
#define __STREAMVIEWER_H_BC589573__

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "GenericEditor.h"

#ifdef _WIN32
#include <windows.h>
#endif

#if JUCE_WINDOWS
#include <gl/gl.h>
#include <gl/glu.h>
#elif JUCE_LINUX
#include <GL/gl.h>
#include <GL/glut.h>
#undef KeyPress
#elif JUCE_IPHONE
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#elif JUCE_MAC
#include <GLUT/glut.h>
#endif

#ifndef GL_BGRA_EXT
#define GL_BGRA_EXT 0x80e1
#endif


class FilterViewport;




class DataWindow : public DocumentWindow
{
public:
	DataWindow();
	~DataWindow();

	void closeButtonPressed();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DataWindow);

};




class StreamViewer : public GenericEditor//,
					 //public Slider::Listener
{
public:
	StreamViewer (GenericProcessor* parentNode, FilterViewport* vp);
	~StreamViewer();
	//void sliderValueChanged (Slider* slider);

private:	
	//Slider* slider;
	ScopedPointer <DataWindow> dataWindow;

	int tabIndex;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StreamViewer);

};




class StreamViewerRenderer : public OpenGLComponent,
							 public ActionListener

{
public:
	StreamViewerRenderer(AudioSampleBuffer* buffer, int channel);
	~StreamViewerRenderer();
	void newOpenGLContextCreated();
	void renderOpenGL();
	void select();
	void deselect();

private:
	AudioSampleBuffer* displayBuffer;

	void actionListenerCallback(const String& msg);

	int channel;
	int isSelected;

};


class StreamList : public ListBox,
				   public ListBoxModel

{
public:
	StreamList()
		: ListBox ("Stream List", 0)
	{
		setModel(this);
		setMultipleSelectionEnabled (true);
		setRowHeight(40);
		//setBounds(0,0,getWidth(),5*20);
		setColour(ListBox::backgroundColourId,Colours::darkgrey);
		setColour(ListBox::outlineColourId,Colours::darkgrey);
	}

	~StreamList()
	{
	}

	int getNumRows()
	{
		return 20;
	}

	void paintListBoxItem (int rowNumber,
						   Graphics& g,
						   int width, 
						   int height,
						   bool rowIsSelected)
	{
		if (rowIsSelected)
			g.fillAll (Colours::lightblue);
		else
			g.fillAll (Colours::black.withAlpha(0.1f));
		
		g.setColour (Colours::yellow);
		g.setFont (height* 0.7f);
		g.drawText (String (rowNumber +1),
					5, 0, width, height,
					Justification::centredLeft, true);

	}

	void paint (Graphics& g)
	{
		g.fillAll (Colours::black);
	}

	Component* refreshComponentForRow (int rowNumber, bool isRowSelected, 
	                                   Component* existingComponentToUpdate)
	{
		StreamViewerRenderer* viewer = (StreamViewerRenderer*) existingComponentToUpdate;

		if (viewer == 0)
			viewer = new StreamViewerRenderer (0, rowNumber);

	
		if (isRowSelected) {
			viewer->select();
			std::cout << "Row " << rowNumber << " is selected." << std::endl;
		} else {
			viewer->deselect();
		}
		
		return viewer;
	}
};


// class StreamViewer : public Component,
// 					 public ActionListener

// {
// public:
// 	StreamViewer(AudioSampleBuffer* buffer);
// 	~StreamViewer();

// 	void resized();
	
// private:
// 	StreamViewerControls* streamViewerControls;
// 	StreamViewerRenderer* streamViewerRenderer;	

// 	void actionListenerCallback(const String& msg);

// };



#endif  // __STREAMVIEWER_H_BC589573__
