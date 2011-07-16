/*
  ==============================================================================

    StreamViewer.cpp
    Created: 1 May 2011 4:48:59pm
    Author:  jsiegle

  ==============================================================================
*/

#include "StreamViewer.h"


StreamViewer::StreamViewer (GenericProcessor* parentNode, FilterViewport* vp) 
	: GenericEditor(parentNode, vp), tabIndex(-1)

{
	desiredWidth = 210;

	//dataWindow = new DataWindow();//(T("Stream Window"), Colours::black, DocumentWindow::allButtons);

	StreamViewerRenderer* streamList = new StreamViewerRenderer(0,0);
	//Viewport* listViewer = new Viewport("Viewport");
	//listViewer->setViewedComponent(streamList);
	//streamList->setBounds(0,0,viewport->getWidth(),viewport->getHeight());

	tabIndex = viewport->addTab("Stream",streamList);
	//listViewer->setBounds(0,0,300,400);
	

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
	//viewport->removeTab(0);
	if (tabIndex > -1)
		viewport->removeTab(tabIndex);

	deleteAllChildren();
	//dataWindow = 0;
	//slider = 0;	

}



//===================================================

StreamViewerRenderer::StreamViewerRenderer(AudioSampleBuffer* buffer, int chan)
	: displayBuffer(buffer), channel(chan), isSelected(false)
{

}

StreamViewerRenderer::~StreamViewerRenderer()
{
	
}

void StreamViewerRenderer::select()
{
	isSelected = true;
	glClearColor(0.0f, 1.0f, 0.25f, 0.0f);
}

void StreamViewerRenderer::deselect()
{
	isSelected = false;
	glClearColor(0.0f, 0.5f, 0.25f, 0.0f);
}

void StreamViewerRenderer::newOpenGLContextCreated()
{
	glClearColor(0.0f, 0.5f, 0.25f, 1.0f);
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

	glColor3f(1,1,1);
	glBegin(GL_LINE_STRIP);
	glVertex2f(0,0);
	glVertex2f(1,1);
	glVertex2f(0,1);
	glVertex2f(1,0);
	glVertex2f(0,0);
	glEnd();


	// int nSamples = displayBuffer->getNumSamples();
	
	// glBegin(GL_LINE_STRIP);

	// glColor3f(0,0,0);//1.0*chan/16,1.0*chan/16,1.0*chan/16);
	
	// for (int n = 0; n < nSamples-skip; n+= skip )
	// {
	// 	glVertex2f(float(n)/nSamples,*displayBuffer->getSampleData(channel,n));
	// 	glVertex2f(float(n+skip)/nSamples,*displayBuffer->getSampleData(channel,n+skip));
	// }
		
	// glEnd();


	glFlush();
		
	
}


void StreamViewerRenderer::actionListenerCallback(const String & msg)
{
	repaint();
}




DataWindow::DataWindow()
	: DocumentWindow ("Stream Window", 
					  Colours::black, 
					  DocumentWindow::allButtons)

{
	centreWithSize(300,200);
	setUsingNativeTitleBar(true);
	setResizable(true,true);
	setVisible(true);
	setTitleBarHeight(40);
	setContentComponent(0);
}

DataWindow::~DataWindow()
{
	//deleteAllChildren();
	//setContentComponent (0);
	
}

void DataWindow::closeButtonPressed()
{
	setVisible(false);
	//viewport->removeTab(0);

}