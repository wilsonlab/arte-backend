/*
  ==============================================================================

    StreamViewer.cpp
    Created: 1 May 2011 4:48:59pm
    Author:  jsiegle

  ==============================================================================
*/

#include "StreamViewer.h"


StreamViewer::StreamViewer (GenericProcessor* parentNode, FilterViewport* vp) 
	: GenericEditor(parentNode, vp)

{
	desiredWidth = 210;

	docWindow = new DocumentWindow(T("Stream Window"), Colours::black, DocumentWindow::allButtons);
	docWindow->centreWithSize(300,200);
	docWindow->setUsingNativeTitleBar(true);
	docWindow->setResizable(true,true);
	docWindow->setVisible(true);

	viewport->addTab("Stream",0);

	//tabIndex = 

}

StreamViewer::~StreamViewer()
{
	//std::cout << "    Filter editor for " << getName() << " being deleted with " << getNumChildComponents() << " children. " << std::endl;
	
	//slider = 0;
	//Slider* s = slider.release();
	//delete s;
	//slider = 0;
	//delete slider;
	
	//slider = 0;

	//delete(docWindow);

	deleteAllChildren();
	//slider = 0;	

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
	glClearColor(0.0f, 0.5f, 0.25f, 0.0f);
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
		glColor3f(0,0,0);//1.0*chan/16,1.0*chan/16,1.0*chan/16);
	
		for (int n = 0; n < nSamples-skip; n+= skip )
		{
			glVertex2f(float(n)/nSamples,*displayBuffer->getSampleData(chan,n)+0.03+chan*0.06);
			glVertex2f(float(n+skip)/nSamples,*displayBuffer->getSampleData(chan,n+skip)+0.03+chan*0.06);
		}
		
		glEnd();

	}

	glFlush();
		
	
}


void StreamViewerRenderer::actionListenerCallback(const String & msg)
{
	repaint();
}