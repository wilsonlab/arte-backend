/*
  ==============================================================================

    LfpDisplayCanvas.h
    Created: 8 Feb 2012 1:06:53pm
    Author:  jsiegle

  ==============================================================================
*/

#ifndef __LFPDISPLAYCANVAS_H_B711873A__
#define __LFPDISPLAYCANVAS_H_B711873A__


#include "../../../JuceLibraryCode/JuceHeader.h"
#include "OpenGLCanvas.h"
#include "../../UI/Configuration.h"

class LfpDisplayCanvas : public OpenGLCanvas

{
public: 
	LfpDisplayCanvas(AudioSampleBuffer* streamBuffer, MidiBuffer* eventBuffer, Configuration* config);
	~LfpDisplayCanvas();
	void newOpenGLContextCreated();
	void renderOpenGL();

private:

	int xBuffer, yBuffer;

	void drawLabel();

	int getTotalHeight();

	void resized();
	void mouseDown(const MouseEvent& e);
	void mouseDrag(const MouseEvent& e);
	void mouseMove(const MouseEvent& e);
	void mouseUp(const MouseEvent& e);
	void mouseWheelMove(const MouseEvent&, float, float);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LfpDisplayCanvas);
	
};



#endif  // __LFPDISPLAYCANVAS_H_B711873A__
