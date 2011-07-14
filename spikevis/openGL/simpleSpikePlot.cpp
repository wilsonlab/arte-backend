#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>	// OpenGL Graphics Utility Library
#include "netcom.h"
#include "datapacket.h"
#include <math.h>
#include <vector>

// These variables control the current mode
int CurrentMode = 2;
const int NumModes = 5;

// These variables set the dimensions of the rectanglar region we wish to view.
const double xMin = 0, xMax = 400;
const double yMin = -1*pow(2,15), yMax = pow(2,15);


char host[] = "localhost";
int port = 6303;

NetComDat net;// = NetCom::initUdpRx(host,port);

spike_net_t spike;
int spikeCount =0;
timeval startTime, now;

int x[32];
int y[32];

void startOpenGL();
void draw();
void drawBoundingBoxes();
void drawWaveforms();
void initRendering();
void resizeWindow(int w, int h);
void getNetSpike();



// Main routine
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{

	net = NetCom::initUdpRx(host,port);
	srand(time(NULL));
	gettimeofday(&startTime,NULL);

	glutInit(&argc,argv);
	startOpenGL();

	return(0);	// This line is never reached.
}

void getNetSpike(void){
	//std::cout<<"Waiting for spike  ";
	NetCom::rxSpike(net, &spike);
	spikeCount++;
	draw();
}

void startOpenGL(){


	// The image is not animated so single buffering is OK. 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

	// Window position (from top corner), and size (width and hieght)
	glutInitWindowPosition( 20, 60 );
	glutInitWindowSize( 300, 125 );
	glutCreateWindow( "Simple Spike Viewer" );

	// Initialize OpenGL as we like it..
	initRendering();

	// Set up the callback function for resizing windows
	glutReshapeFunc( resizeWindow );

	// Call this for background processing
	 glutIdleFunc( getNetSpike );

	// call this whenever window needs redrawing
	glutDisplayFunc( draw );

	// Start the main loop.  glutMainLoop never returns.
	glutMainLoop(  );

}


// Initialize OpenGL's rendering modes
void initRendering()
{
	glEnable ( GL_DEPTH_TEST );
	glEnableClientState(GL_VERTEX_ARRAY);
}

// Called when the window is resized
//		w, h - width and height of the window in pixels.


void draw(void)
{
	//std::cout<<"Drawing Spike!  ";
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f( 1.0, 1.0, 1.0 );		

	drawBoundingBoxes();

	drawWaveforms();

	glutSwapBuffers();
	glFlush();
}
void drawWaveforms(void){

	int buffSize = spike.n_chans * spike.n_samps_per_chan;
	int shift = pow(2,15);
	shift = 0;

	for (int i=0; i<buffSize; i++)
		spike.data[i] = spike.data[i]-shift;

	float dx = (xMax -xMin)/(float)(buffSize-2);

	//spike.data[0] = 0 + spikeCount%50 * yMax/50;
	//spike.data[1] = spike.data[0];


	int sampIdx = 0;
	glLineWidth(3.0);

	// ================== Trace 1
	glColor3f( 1.0, 0.0, 0.0 );		
	sampIdx = 0;
	glBegin( GL_LINE_STRIP );
		for (int i=0; i<spike.n_samps_per_chan; i++)
		{
			glVertex2f(((i + 0*spike.n_samps_per_chan)*dx)+xMin, spike.data[sampIdx]);
			sampIdx +=4;
		}
	glEnd();

	// ================== Trace 2
	glColor3f( 1.0, 1.0, 0.0 );		
	sampIdx = 1;
	glBegin( GL_LINE_STRIP );
		for (int i=0; i<spike.n_samps_per_chan; i++)
		{
			glVertex2f(((i + 1*spike.n_samps_per_chan)*dx)+xMin, spike.data[sampIdx]);
			sampIdx +=4;
		}
	glEnd();

	// ================== Trace 3
	glColor3f( 0.0, 1.0, 0.0 );		
	sampIdx = 2;
	glBegin( GL_LINE_STRIP );
		for (int i=0; i<spike.n_samps_per_chan; i++)
		{
			glVertex2f(((i + 2*spike.n_samps_per_chan)*dx)+xMin, spike.data[sampIdx]);
			sampIdx +=4;
		}
	glEnd();

	// ================== Trace 4
	glColor3f( 0.0, 1.0, 1.0 );		
	sampIdx = 3;
	glBegin( GL_LINE_STRIP );
		for (int i=0; i<spike.n_samps_per_chan; i++)
		{
			glVertex2f(((i + 3*spike.n_samps_per_chan)*dx)+xMin, spike.data[sampIdx]);
			sampIdx +=4;
		}
	glEnd();



	gettimeofday(&now,NULL);

	long dt = (now.tv_usec - startTime.tv_usec);
	if (spikeCount%10000==0)
		std::cout<<"Spike count:" << spikeCount<<"  DT"<<dt<<"  Frame rate:"<<1e6/dt<<std::endl;
	startTime.tv_usec = now.tv_usec;;


}
void drawBoundingBoxes(void){
	
	glLineWidth(1.0);
	glColor3f( 1.0, 1.0, 1.0 );		
	glBegin(GL_LINE_LOOP);
		glVertex2f(xMin+1, yMin+1);
		glVertex2f(xMin+1, yMax);
		glVertex2f(xMax * 1.0/4.0, yMax);
		glVertex2f(xMax * 1.0/4.0, yMin+.2);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex2f(xMin+1, yMin+1);
		glVertex2f(xMin+1, yMax);
		glVertex2f(xMax * 2.0/4.0, yMax);
		glVertex2f(xMax * 2.0/4.0, yMin+1);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex2f(xMin+1, yMin+1);
		glVertex2f(xMin+1, yMax);
		glVertex2f(xMax * 3.0/4.0, yMax);
		glVertex2f(xMax * 3.0/4.0, yMin+1);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex2f(xMin+1, yMin+1);
		glVertex2f(xMin+1, yMax);
		glVertex2f(xMax * 4.0/4.0, yMax);
		glVertex2f(xMax * 4.0/4.0, yMin+1);
	glEnd();
}

void resizeWindow(int w, int h)
{


	double scale, center;
	double windowxMin, windowxMax, windowyMin, windowyMax;

	// Define the portion of the window used for OpenGL rendering.
	glViewport( 0, 0, w, h );	// View port uses whole window

	// Set up the projection view matrix: orthographic projection
	// Determine the min and max values for x and y that should appear in the window.
	// The complication is that the aspect ratio of the window may not match the
	//		aspect ratio of the scene we want to view.
	w = (w==0) ? 1 : w;
	h = (h==0) ? 1 : h;
	if ( (xMax-xMin)/w < (yMax-yMin)/h ) {
		scale = ((yMax-yMin)/h)/((xMax-xMin)/w);
		center = (xMax+xMin)/2;
		windowxMin = center - (center-xMin)*scale;
		windowxMax = center + (xMax-center)*scale;
		windowyMin = yMin;
		windowyMax = yMax;
	}
	else {
		scale = ((xMax-xMin)/w)/((yMax-yMin)/h);
		center = (yMax+yMin)/2;
		windowyMin = center - (center-yMin)*scale;
		windowyMax = center + (yMax-center)*scale;
		windowxMin = xMin;
		windowxMax = xMax;
	}
	
	// Now that we know the max & min values for x & y that should be visible in the window,
	//		we set up the orthographic projection.

	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( xMin, xMax, yMin, yMax, -1, 1 );

}

