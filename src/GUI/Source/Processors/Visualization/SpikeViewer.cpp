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


void SpikeViewer::newOpenGLContextCreated()
{
	glClearColor(0.9f, 0.7f, 0.2f, 1.0f);
	glClearDepth (1.0);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho(0, 1, 1, 0, 0, 1);
	glMatrixMode (GL_MODELVIEW);

	// glEnable (GL_BLEND);
	// glShadeModel(GL_FLAT);
	
	// glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
	// glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);
	// glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// glColor3f(1,1,1);

	glFlush();

	glClear(GL_COLOR_BUFFER_BIT);

}


void SpikeViewer::renderOpenGL()
{
		
	 if (eventBuffer->getNumEvents() > 0) {

	 	glClear(GL_COLOR_BUFFER_BIT);

	   	glViewport(0,0,getWidth()/2,getHeight());

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

			//std::cout << " Bytes received: " << numbytes << std::endl;
			//std::cout << " Message timestamp = " << message.getTimeStamp() << std::endl;
			//std::cout << " Message channel: " << chan << std::endl;

 			//std::cout << "   ";

 			dataptr += 2;

 			float spikeData[numSamples];

 			glColor3f(0,0.0,0);
			glBegin(GL_LINE_STRIP);

 			for (int n = 0; n < numSamples; n++)
 			{
 				uint16 sampleValue = (*dataptr << 8) + *(dataptr+1);

 				//spikeData[n] = float(sampleValue - 32768);
 				glVertex2f(float(n)/numSamples, 
 						   -float(sampleValue - 32768)/32768 + 0.6);

 				dataptr += 2;

 			}

			glEnd();

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

