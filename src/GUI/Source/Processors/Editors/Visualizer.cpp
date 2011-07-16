/*
  ==============================================================================

    Visualizer.cpp
    Created: 15 Jul 2011 8:42:01pm
    Author:  jsiegle

  ==============================================================================
*/

#include "Visualizer.h"

SelectorButton::SelectorButton()
	: DrawableButton (T("Selector"), DrawableButton::ImageFitted)
{
	DrawablePath normal, over, down;

	    Path p;
        p.addEllipse (0.0,0.0,20.0,20.0);
        normal.setPath (p);
        normal.setFill (Colours::lightgrey);
        normal.setStrokeThickness (0.0f);

        over.setPath (p);
        over.setFill (Colours::black);
        over.setStrokeFill (Colours::black);
        over.setStrokeThickness (5.0f);

        setImages (&normal, &over, &over);
        setBackgroundColours(Colours::darkgrey, Colours::green);
        setClickingTogglesState (true);
        setTooltip ("Toggle a state.");

}

SelectorButton::~SelectorButton()
{
}


Visualizer::Visualizer (GenericProcessor* parentNode, FilterViewport* vp) 
	: GenericEditor(parentNode, vp), tabIndex(-1), dataWindow(0)

{
	desiredWidth = 210;

	windowSelector = new SelectorButton();
	windowSelector->addListener(this);
	windowSelector->setBounds(25,25,20,20);
	windowSelector->setToggleState(false,false);
	addAndMakeVisible(windowSelector);

	tabSelector = new SelectorButton();
	tabSelector->addListener(this);
	tabSelector->setBounds(25,50,20,20);
	tabSelector->setToggleState(false,false);
	addAndMakeVisible(tabSelector);


}

Visualizer::~Visualizer()
{

	if (tabIndex > -1)
		viewport->removeTab(tabIndex);

	deleteAllChildren();

}

void Visualizer::buttonClicked(Button* button)
{
	if (button == windowSelector)
	{
		if (dataWindow == 0) {
			dataWindow = new DataWindow(windowSelector);
			dataWindow->setContentComponent(new Renderer(0,0));
			dataWindow->setVisible(true);
			
		} else {
			dataWindow->setVisible(windowSelector->getToggleState());
		}

	} else if (button == tabSelector)
	{
		if (tabSelector->getToggleState() && tabIndex < 0)
		{
			tabIndex = viewport->addTab("Visualizer",new Renderer(0,0));
		} else if (!tabSelector->getToggleState() && tabIndex > -1)
		{
			viewport->removeTab(tabIndex);
			tabIndex = -1;
		}
	}
	
}

//===================================================

Renderer::Renderer(AudioSampleBuffer* sBuffer, MidiBuffer* eBuffer)
	: streamBuffer(sBuffer), eventBuffer(eBuffer)
{

}

Renderer::~Renderer()
{
	
}


void Renderer::newOpenGLContextCreated()
{
	glClearColor(0.9f, 0.5f, 0.25f, 1.0f);
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


void Renderer::renderOpenGL()
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


void Renderer::actionListenerCallback(const String & msg)
{
	repaint();
}




DataWindow::DataWindow(Button* cButton)
	: DocumentWindow ("Stream Window", 
					  Colours::black, 
					  DocumentWindow::allButtons),
	  controlButton(cButton)

{
	centreWithSize(300,200);
	setUsingNativeTitleBar(true);
	setResizable(true,true);
	setTitleBarHeight(40);
}

DataWindow::~DataWindow()
{
	//deleteAllChildren();
	setContentComponent (0);
	
}

void DataWindow::closeButtonPressed()
{
	setVisible(false);
	controlButton->setToggleState(false,false);
	//viewport->removeTab(0);

}