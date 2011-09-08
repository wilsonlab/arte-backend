/*
  ==============================================================================

    SpikeViewer.cpp
    Created: 16 Aug 2011 8:34:41pm
    Author:  jsiegle

  ==============================================================================
*/

#include "SpikeViewer.h"


SpikeViewer::SpikeViewer(AudioSampleBuffer* sBuffer, MidiBuffer* eBuffer, UIComponent* ui)
	: Renderer(sBuffer, eBuffer, ui)
{

}



SpikeViewer::~SpikeViewer() {}

void SpikeViewer::resized() 
{
	
	xBox = getWidth()/3;
	yBox = getHeight()/2;
	yPadding = 0.05;
	xPadding = 0.05;

	repaint();

}

void SpikeViewer::newOpenGLContextCreated()
{

	glClearDepth (1.0);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho(0, 1, 1, 0, 0, 1);
	glMatrixMode (GL_MODELVIEW);


	//glClear(GL_COLOR_BUFFER_BIT);

	//glViewport(0,0,getWidth()/2,getHeight());

	//glColor3f(1,1,1);

	
	glClearColor(0.9f, 0.7f, 0.2f, 1.0f);
	glColor3f(0,0.0,0);

	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_LINE_STRIP);
 	glVertex2f(0.01f, 0.01f);
 	glVertex2f(0.99f, 0.99f);
 	glEnd();


	glFlush();

}


void SpikeViewer::renderOpenGL()
{
		
	 if (eventBuffer->getNumEvents() > 0) {

	   	glClear(GL_COLOR_BUFFER_BIT);

	   	glRasterPos2f(0.1,0.1);

	   	//const char* str = "i";
	  // 	void* font = GLUT_BITMAP_8_BY_13;

	  // 	glutBitmapCharacter(font,54);
	   //	drawBorder();

		//std::cout << "Events received by Spike Viewer." << std::endl;

		MidiBuffer::Iterator i (*eventBuffer);
		MidiMessage message(0xf4);

		int samplePosition;
		i.setNextSamplePosition(samplePosition);

		while (i.getNextEvent (message, samplePosition)) {

			int numbytes = message.getRawDataSize();
			int numSamples = (numbytes-2)/2;
			uint8* dataptr = message.getRawData();

			int chan = (*dataptr<<8) + *(dataptr+1);

 			dataptr += 2;

			glViewport(0,0,getWidth()/2,getHeight());

			if (chan < 4)
			{
			//for (int n = 0; n < 4; n++) {
				setViewportForWaveN(chan);
				drawWaveform(dataptr, numSamples);
				drawBorder();
			}

			//std::cout << " Bytes received: " << numbytes << std::endl;
			//std::cout << " Message timestamp = " << message.getTimeStamp() << std::endl;
			//std::cout << " Message channel: " << chan << std::endl;

 			//std::cout << "   ";
			
 			//AudioDataConverters::convertInt16BEToFloat ( dataptr, // source
    		//			spikeData, // dest
    		//			numSamples, // numSamples
    		//			2 ); // destBytesPerSample = 2

			//for (int n = 0; n < numSamples; n++) {
			//	std::cout << String(spikeData[n]) << " ";
			//}
					
			//std::cout << std::endl << std::endl;
		}

		eventBuffer->clear();

	}

	//glOrtho(0, 0.5, 0.5, 0, 0, 1);
	glFlush();

}

void SpikeViewer::drawWaveform(uint8* dataptr, int numSamples)
{
	
	glBegin(GL_LINE_STRIP);

 	for (int n = 0; n < numSamples; n++)
 	{
 		uint16 sampleValue = (*dataptr << 8) + *(dataptr+1);
 		glVertex2f(float(n)/numSamples + 0.01, 
 					-float(sampleValue - 32768)/32768 + 0.6);
 		dataptr += 2;

 	}

	glEnd();

}

void SpikeViewer::drawBorder()
{
	 glBegin(GL_LINE_STRIP);
 	 glVertex2f(0.01f, 0.01f);
 	 glVertex2f(0.99f, 0.01f);
 	 glVertex2f(0.99f, 0.99f);
 	 glVertex2f(0.01f, 0.99f);
 	 glVertex2f(0.01f, 0.01f);
 	 glEnd();
}

void SpikeViewer::setViewportForProjectionN(int n)
{
	//	std::cout<<"Setting viewport on projection:"<<n<<std::endl;
    float viewDX = xBox;
    float viewDY = yBox;
    float viewX,viewY;

    switch (n){
    case 0:
        viewX=xBox;
        viewY=yBox;
        break;
    case 1:
        viewX = xBox*2;
        viewY = yBox;
        break;
    case 2:
        viewX=xBox*3;
        viewY=yBox;
        break;
    case 3:
        viewX = xBox;
        viewY = 0;
        break;
    case 4:
        viewX = xBox*2;
        viewY = 0;
        break;
    case 5:
        viewX = xBox*3;
        viewY = 0;
        break;
    default:
        std::cout<<"drawing of more than 4 channels is not supported, returning! Requested:"<<n<<std::endl;
        return;
    }
	viewX = viewX + xPadding;
	viewY = viewY + yPadding;
	viewDX = viewDX - 2*xPadding;
	viewDY = viewDY - 2*yPadding;

	glViewport(viewX, viewY, viewDX, viewDY);

}


void SpikeViewer::setViewportForWaveN(int n){
	float viewDX = xBox/2;
	float viewDY = yBox;
	float viewX,viewY;
	switch (n){
	case 0:
		viewX=0;
		viewY=yBox;
		break;
	case 1:
		viewX = xBox/2;
		viewY = yBox;
		break;
	case 2:
		viewX=0;
		viewY=0;
		break;
	case 3:
		viewX = xBox/2;
		viewY = 0;
		break;
	default:
		std::cout<<"drawing of more than 4 channels is not supported, returning! Requested:"<<n<<std::endl;
		return;
	}
	viewX = viewX + xPadding;
	viewY = viewY + yPadding;
	viewDX = viewDX - 2*xPadding;
	viewDY = viewDY - 2*yPadding;
	glViewport(viewX,viewY,viewDX,viewDY);
}