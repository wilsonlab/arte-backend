/*
  ==============================================================================

    LfpDisplayCanvas.cpp
    Created: 8 Feb 2012 1:06:53pm
    Author:  jsiegle

  ==============================================================================
*/

#include "LfpDisplayCanvas.h"

LfpDisplayCanvas::LfpDisplayCanvas(AudioSampleBuffer* streamBuffer, MidiBuffer* eventBuffer, Configuration* config)
	 : xBuffer(10), yBuffer(10)
{
	
}

LfpDisplayCanvas::~LfpDisplayCanvas()
{

}


void LfpDisplayCanvas::newOpenGLContextCreated()
{

	setUp2DCanvas();
	activateAntiAliasing();

	glClearColor (0.667, 0.698, 0.718, 1.0);
	resized();


}

void LfpDisplayCanvas::renderOpenGL()
{
	
	glClear(GL_COLOR_BUFFER_BIT); // clear buffers to preset values

	//drawLabel();

	drawScrollBars();
}

int LfpDisplayCanvas::getTotalHeight() 
{
	return 300;
}


void LfpDisplayCanvas::mouseDown(const MouseEvent& e) 
{

	mouseDownInCanvas(e);
}

void LfpDisplayCanvas::mouseDrag(const MouseEvent& e) {mouseDragInCanvas(e);}
void LfpDisplayCanvas::mouseMove(const MouseEvent& e) {mouseMoveInCanvas(e);}
void LfpDisplayCanvas::mouseUp(const MouseEvent& e) 	{mouseUpInCanvas(e);}
void LfpDisplayCanvas::mouseWheelMove(const MouseEvent& e, float a, float b) {mouseWheelMoveInCanvas(e,a,b);}

void LfpDisplayCanvas::resized()
{
	canvasWasResized();
}