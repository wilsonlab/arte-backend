/*
  ==============================================================================

    StreamViewer.cpp
    Created: 1 May 2011 4:48:59pm
    Author:  jsiegle

  ==============================================================================
*/

#include "StreamViewer.h"

StreamViewerControls::StreamViewerControls()
{
	//slider1 = new Slider();
	//slider1->setBounds(0,0,200,20);
	//addAndMakeVisible(slider1);

	//slider2 = new Slider();
	//slider2->setBounds(0,20,200,20);
	//addAndMakeVisible(slider2);

}

StreamViewerControls::~StreamViewerControls()
{
	deleteAllChildren();
}


//===================================================

StreamViewerRenderer::StreamViewerRenderer(AudioSampleBuffer* buffer)
	: displayBuffer(buffer)
{

}

StreamViewerRenderer::~StreamViewerRenderer()
{
	
}

void StreamViewerRenderer::newOpenGLContextCreated()
{
	glClearColor(0.75f, 0.5f, 0.25f, 0.0f);
	glClearDepth (1.0);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho(0, 1, 1, 0, 0, 1);
	glMatrixMode (GL_MODELVIEW);

		
	//glDepthFunc (GL_LESS);
	//glEnable (GL_DEPTH_TEST);
	//glEnable (GL_TEXTURE_2D);
	glEnable (GL_BLEND);
	glShadeModel(GL_FLAT);
	
	glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor3f(1,1,1);

	glFlush();


}


void StreamViewerRenderer::renderOpenGL()
{

	//std::cout << "Painting..." << std::endl;

	glClear(GL_COLOR_BUFFER_BIT);

	//glMatrixMode (GL_PROJECTION);
	//glLoadIdentity ();
	//glOrtho(0, 1, 1, 0, 0, 1);
	//glMatrixMode (GL_MODELVIEW);


	

	int skip = 1;

	//glBegin(GL_LINE_STRIP);

	int nSamples = displayBuffer->getNumSamples();
	//std::cout << displayBuffer->getNumChannels() << std::endl;

	//std::cout << *displayBuffer->getSampleData(0,1) << std::endl;

	for (int chan = 0; chan < displayBuffer->getNumChannels(); chan++) {
	
		glBegin(GL_LINE_STRIP);

		//std::cout << "Message Received." << std::endl;
		glColor3f(1.0*chan/16,1.0*chan/16,1.0*chan/16);
	
		for (int n = 0; n < nSamples-skip; n+= skip )
		{
			glVertex2f(float(n)/nSamples,*displayBuffer->getSampleData(chan,n)+0.03+chan*0.06);
			glVertex2f(float(n+skip)/nSamples,*displayBuffer->getSampleData(chan,n+skip)+0.03+chan*0.06);
		}
		
		glEnd();

	}

	glFlush();
		
	
}

//===================================================

StreamViewer::StreamViewer(AudioSampleBuffer* buffer)
{
	streamViewerRenderer = new StreamViewerRenderer(buffer);
	streamViewerControls = new StreamViewerControls();

	addAndMakeVisible(streamViewerControls);
	addAndMakeVisible(streamViewerRenderer);
	//streamViewerRenderer->repaint();

	//resized();

}

StreamViewer::~StreamViewer()
{
	deleteAllChildren();
}

void StreamViewer::resized()
{

	int w = getWidth();
	int h = getHeight();

	if (streamViewerRenderer != 0)
		streamViewerRenderer->setBounds(0,0,w-200,h);
	
	if (streamViewerControls != 0)
		streamViewerControls->setBounds(w-200,0,200,40);
	
}

void StreamViewer::actionListenerCallback(const String & msg)
{
	//std::cout << "Message Received." << std::endl;
	streamViewerRenderer->repaint();
}