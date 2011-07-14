#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>	// OpenGL Graphics Utility Library
#include "netcom.h"
#include "datapacket.h"
#include <math.h>
#include <vector>

// These variables set the dimensions of the rectanglar region we wish to view.

static double winWidth = 624, winHeight = 312;
static double xBox = winWidth/4;
static double yBox = winHeight/2;

const double MAX_VOLT = pow(2,15);


// ===================================
// 		Arte Specific Variables
// ===================================

char host[] = "localhost";
int port = 6303;
NetComDat net;// = NetCom::initUdpRx(host,port);
spike_net_t spike;
int spikeCount =0;
timeval startTime, now;

int x[32];
int y[32];

// ===================================
// 		Function Defs
// ===================================

void refreshDrawing();
void drawBoundingBoxes();
void drawWaveforms();
void drawWaveformN(int n);

void resizeWindow(int w, int h);

void getNetSpike();



int main( int argc, char** argv )
{
	if (argc>1)
		port = atoi(argv[1]);
	else
	{
		std::cout<<"Usage: arteSpikeViewer port"<<std::endl;
		return 0;
	}

	net = NetCom::initUdpRx(host,port);
	srand(time(NULL));
	gettimeofday(&startTime,NULL);

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	glDisable(GL_DEPTH_TEST);

	glutInitWindowPosition( 20, 60 );
	glutInitWindowSize( winWidth, winHeight );

	glutCreateWindow( "Arte Network Spike Viewer" );

	glutReshapeFunc( resizeWindow );
	glutIdleFunc( getNetSpike );
	glutDisplayFunc( refreshDrawing );

	glutMainLoop(  );

	return(0);
}

void getNetSpike(void){
	NetCom::rxSpike(net, &spike);
	spikeCount++;
	refreshDrawing();
}


void refreshDrawing(void)
{
	//std::cout<<"Drawing Spike!  ";
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f( 1.0, 1.0, 1.0 );		

	drawWaveforms();
	drawBoundingBoxes();

	glutSwapBuffers();
	glFlush();
}
void drawWaveforms(void){

	int buffSize = spike.n_chans * spike.n_samps_per_chan;
	int shift = pow(2,14);

	for (int i=0; i<buffSize; i++)
		spike.data[i] = spike.data[i]-shift;

	glLineWidth(3.0);


	// ================== Trace 1 ===========
	//glScissor(0,0,winWidth/2, winHeight/2);	
	glLoadIdentity();
	for (int i=0; i<spike.n_chans; i++)
		drawWaveformN(i);


	gettimeofday(&now,NULL);

	long dt = (now.tv_usec - startTime.tv_usec);
	if (spikeCount%10000==0)
		std::cout<<"Spike count:" << spikeCount<<"  DT"<<dt<<"  Frame rate:"<<1e6/dt<<std::endl;
	startTime.tv_usec = now.tv_usec;;


}

void drawWaveformN(int n)
{
	int sampIdx = n;

	float viewDX = xBox/2;
	float viewDY = yBox;
	float viewX,viewY;
	switch (n){
	case 0:
		glColor3f( 1.0, 0.0, 0.0);
		viewX=0;
		viewY=yBox;
		break;
	case 1:
		glColor3f( 1.0, 1.0, 0.0);
		viewX = xBox/2;
		viewY = yBox;
		break;
	case 2:
		glColor3f( 0.0, 1.0, 0.0);
		viewX=0;
		viewY=0;
		break;
	case 3:
		glColor3f( 0.0, 1.0, 1.0);
		viewX = xBox/2;
		viewY = 0;
		break;
	default:
		std::cout<<"drawing of more than 4 channels is not supported, returning! Requested:"<<n<<std::endl;
		return;
	}
	glViewport(viewX,viewY,viewDX,viewDY);


	float x = -1;
	float dx = 2.0/(spike.n_samps_per_chan-1);
	float dy = 2.0/((float)MAX_VOLT*2);
	glBegin( GL_LINE_STRIP );
	for (int i=0; i<spike.n_samps_per_chan; i++)
	{
		glVertex2f(x, spike.data[sampIdx]*dy);
		sampIdx +=4;
		x +=dx;
	}
	glEnd();
}




void drawBoundingBoxes(void){
	
	glLineWidth(1.0);
	glColor3f( 1.0, 1.0, 1.0 );		

	// --------------------------------
	// Draw Bounding Box for Waveforms
	// --------------------------------
	glViewport(0, 0, xBox, yBox*2	);
	glBegin(GL_LINES);
		//Horizontal Line
		glVertex2f(-1, 0);
		glVertex2f(1,  0);
		//Vertical Line
		glVertex2f(0, -1);
		glVertex2f(0,  1);
	glEnd();

	glLineWidth(5);

	//Right Edge LIne
	glBegin(GL_LINES);
		glVertex2f(1, -1);
		glVertex2f(1,  1);
	glEnd();


	// --------------------------------
	// Draw Bounding Box for projections
	// --------------------------------
	glViewport(xBox, 0, 3*xBox, yBox*2);
	glLineWidth(1);
	glBegin(GL_LINES);
		//Horizontal Line
		glVertex2f(-1, 0);
		glVertex2f(1,  0); 
		//left vertical line
		glVertex2f(-1.0/3.0, -1);
		glVertex2f(-1.0/3.0,  1);
		//right vertical line
		glVertex2f(1.0/3.0, -1);
		glVertex2f(1.0/3.0,  1);
	glEnd();

}

void resizeWindow(int w, int h)
{

	winWidth = w;
	winHeight = h;
	xBox = w/4;
	yBox = h/2;
	double scale, center;
	double windowxMin, windowxMax, windowyMin, windowyMax;

	// Define the portion of the window used for OpenGL rendering.
	glViewport( 0, 0, w, h );	// View port uses whole window

	// Set up the projection view matrix: orthographic projection
	// Determine the min and max values for x and y that should appear in the window.
	// The complication is that the aspect ratio of the window may not match the
	//		aspect ratio of the scene we want to view.
/*
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

*/	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, 0, winWidth, winHeight, 0, 0 );

}

