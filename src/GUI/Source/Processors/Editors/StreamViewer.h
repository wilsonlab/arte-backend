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

class StreamViewer : public GenericEditor//,
					 //public Slider::Listener
{
public:
	StreamViewer (GenericProcessor* parentNode, FilterViewport* vp);
	~StreamViewer();
	//void sliderValueChanged (Slider* slider);

private:	
	//Slider* slider;
	DocumentWindow* docWindow;

	int tabIndex;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StreamViewer);

};


class StreamViewerRenderer : public OpenGLComponent,
							 public ActionListener

{
public:
	StreamViewerRenderer(AudioSampleBuffer* buffer);
	~StreamViewerRenderer();
	void newOpenGLContextCreated();
	void renderOpenGL();

private:
	AudioSampleBuffer* displayBuffer;

	void actionListenerCallback(const String& msg);

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
