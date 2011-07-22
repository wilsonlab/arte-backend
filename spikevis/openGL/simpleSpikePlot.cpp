#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>	// OpenGL Graphics Utility Library
#include "netcom.h"
#include "datapacket.h"
#include <math.h>
#include <vector>

// these offsets create padding between the different plots
// making a more pleasing display

const double MAX_VOLT = pow(2,15);

// ===================================
// 		GUI Specific Variables
// ===================================
static double winWidth = 624, winHeight = 312;
static double commandWinHeight = 20; // Shift the entire UI up by commandWinHeight and reserve this area for text and buttons

static double xBox = winWidth/4;
static double yBox = winHeight/2;

static double xPadding = 2;
static double yPadding = 2;

static double waveformLineWidth = 1;
static float waveformColor[3] = {1.0, 0.0, 0.0};

static bool clearWave = true;

static char txtDispBuff[40];

void *font = GLUT_BITMAP_8_BY_13;

// Scaling Variables
// Defines how much to shift the waveform within the viewport
static float dV = 1.0/((float)MAX_VOLT*2);
static double scaleShift = -.75;
static float userScale = 1;
static float dUserScale = .3;
// ===================================
// 		Arte Specific Variables
// ===================================

static char host[] = "localhost";
static int port = 6303;
static NetComDat net; // = NetCom::initUdpRx(host,port);
static spike_net_t spike;

// ===================================
// 		Data Format Specific Variables
// ===================================
static int nChan=4;
static int nProj=6;

// ===================================
// 		Misc Variables
// ===================================
static int spikeCount =0;
static timeval startTime, now;

static unsigned char msg[250];
static int const msgLen = 250;
static int cIdx = 0;

// ===================================
// 		General Function Headers
// ===================================

void initTextures();
void refreshDrawing();
void drawBoundingBoxes();
void eraseWaveforms();
void eraseCommandString();

void setViewportForWaveN(int n);
void setViewportForProjectionN(int n);
void setViewportForCommandString();

void drawViewportEdge();

void drawWaveforms();
void drawWaveformN(int n);

void drawProjections();
void drawProjectionN(int n, int idx);
int calcWaveMaxInd();

void resizeWindow(int w, int h);

float scaleVoltage(int v);
// ===================================
// 		Keyboard & Command Function Headers
// ===================================

void toggleOverlay();
void clearWindow();

void keyPressedFn(unsigned char key, int x, int y);
void dispCommandString();

bool executeCommand(unsigned char * cmd);

void drawString(float x, float y, char *string);
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

	bzero(msg, msgLen);
	net = NetCom::initUdpRx(host,port);
	srand(time(NULL));
	gettimeofday(&startTime,NULL);

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	glDisable(GL_DEPTH_TEST);

	glutInitWindowPosition( 20, 60 );
	glutInitWindowSize( winWidth, winHeight);

	char windowTitle[100]; 

	sprintf(windowTitle, "Arte Spike Viewer: %d", port);

	glutCreateWindow(windowTitle);
	glutReshapeFunc( resizeWindow );
	glutIdleFunc( getNetSpike );
	glutDisplayFunc( refreshDrawing );
	glutKeyboardFunc(keyPressedFn);	

	
	std::cout<<"Starting the GLUT main Loop"<<std::endl;
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
//	std::cout<<" ------ Refresh Drawing -------"<<std::endl;

//	glClear(GL_COLOR_BUFFER_BIT);

	if (clearWave)
		eraseWaveforms();

	drawWaveforms();
	drawProjections();
	drawBoundingBoxes();

	// draw the bounding boxes last so they are "ON TOP" of everything else
	dispCommandString();
	glutSwapBuffers();
	glFlush();

	long dt = (now.tv_usec - startTime.tv_usec);
	if (spikeCount%1000==0)
		std::cout<<"Spike count:" << spikeCount<<"  DT"<<dt<<"  Frame rate:"<<1e6/dt<<std::endl;
	startTime.tv_usec = now.tv_usec;;

}



void eraseWaveforms(){
// Note that we only want to erase the waveforms not the projections.
// the projections are simply drawn as points and not state is saved 

	for (int i=0; i<nChan; i++)
	{
		setViewportForWaveN(i);
		glColor3f(0,0,0);
		glRectf(-1, -1, 2, 2);
	}

}

void eraseCommandString(){
	setViewportForCommandString();
	glColor3f(0,0,0);
	glRectf(-1, -1, 2, 20);
}


void drawWaveforms(void){

//	std::cout<<"Drawing waveforms"<<std::endl;

	int buffSize = spike.n_chans * spike.n_samps_per_chan;

	glLineWidth(waveformLineWidth);

	for (int i=0; i<spike.n_chans; i++) 
			drawWaveformN(i);

	gettimeofday(&now,NULL);

}


void drawWaveformN(int n)
{
	setViewportForWaveN(n);

	// Disp the threshold value
	int thresh = spike.thresh[n];
	sprintf(txtDispBuff, "T:%d", thresh);
	glColor3f(1.0,1.0,1.0);
	drawString(-.9, .8, txtDispBuff);

	// Draw the actual waveform
	float dx = 2.0/(spike.n_samps_per_chan-1);
	float x = -1;
	int	sampIdx = n;
	glColor3f(1.0, 1.0, 0.6);
	glBegin( GL_LINE_STRIP );
		for (int i=0; i<spike.n_samps_per_chan; i++)
		{
			glVertex2f(x, scaleVoltage(spike.data[sampIdx]));
			sampIdx +=4;
			x +=dx;
		}
	glEnd();

	// Draw the threshold line
	glColor3f(1.0, 0.0, 0.0);
	glLineStipple(4, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);
	glBegin( GL_LINE_STRIP );
		glVertex2f(-1.0, scaleVoltage(thresh));
		glVertex2f( 1.0, scaleVoltage(thresh));
	glEnd();		
	glDisable(GL_LINE_STIPPLE);
}


void setViewportForWaveN(int n){
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

	//std::cout<<"Setting waveform viewport N:"<<n<<" X:"<<viewX<<" Y:"<<viewY<<" dx:"<<viewDX<<" dy:"<<viewDY<<std::endl;

	glViewport(viewX,viewY,viewDX,viewDY);

}


void setViewportForProjectionN(int n){
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
void setViewportForCommandString(){

    float viewX = 0 + xPadding;
	float viewY = 0 + yPadding;
    float viewDX = winWidth - 2*xPadding;
    float viewDY =  commandWinHeight;

	glViewport(viewX, viewY, viewDX, viewDY);
}



void drawProjections(){

	int maxIdx = calcWaveMaxInd();

	//std::cout<<"Wave Max Ind:"<<maxIdx<<" on Channel:"<<maxIdx%4<<std::endl;	
	maxIdx = maxIdx  - maxIdx%4;  
	// shift the index of the peak waveform to the first channel
    // this should make future indexing  less confusing

	// MAGIC NUMBER HACK, YUCK!!!!! I NEED TO FIX THIS WITH PROPER LOGIC
	for (int i=0; i<6; i++) // <----------------------------------------
		drawProjectionN(i, maxIdx);

}


void drawProjectionN(int n, int idx){

	setViewportForProjectionN(n);

	int d1, d2;
	if (n==0){
		d1 = 0;
		d2 = 1;
	}
	else if(n==1){
		d1 = 0;
		d2 = 2;
	}
	else if(n==2){
		d1 = 0;
		d2 = 3;
	}
	else if(n==3){
		d1 = 1;
		d2 = 2;
	}
	else if(n==4){
		d1 = 1;
		d2 = 3;
	}
	else if (n==5){
		d1 = 2;
		d2 = 3;
	}
	else{
		d1 = 0;
		d2 = 1;
		std::cout<<"Invalid projection combination selected, please restrict value to 0:5"<<std::endl;
	}

	glColor3f( 1.0, 1.0, 1.0 );
//	std::cout<<"Plotting points:"<<spike.data[idx+d1]*dx<<" "<<spike.data[idx+d2]*dy<<std::endl;
	glBegin(GL_POINTS);
//		glVertex2f(spike.data[idx+d1], spike.data[idx+d2]);
		glVertex2f(scaleVoltage(spike.data[idx+d1]), scaleVoltage(spike.data[idx+d2]));
	glEnd();
}


int  calcWaveMaxInd(){
	int idx = -1;
	int val = -1*2^15;
	for (int i=0; i<spike.n_samps_per_chan * spike.n_chans; i++)
		if(val < spike.data[i])
		{
			idx = i;
			val = spike.data[i];
		}
	return idx;
}



void drawBoundingBoxes(void){

//	std::cout<<"Drawing bounding boxes"<<std::endl;

	glColor3f(1.0, 1.0, 1.0);
	for (int i=0; i<nChan; i++)
	{
		setViewportForWaveN(i);
		drawViewportEdge();
	}
	for (int i=0; i<nProj; i++)
	{
		setViewportForProjectionN(i);
		drawViewportEdge();
	}
}



void drawViewportEdge(){
	glBegin(GL_LINE_LOOP);
		glVertex2f(-.999, -.999);
		glVertex2f( .999, -.999);
		glVertex2f( .999, .999);
		glVertex2f(-.999, .999);
	glEnd();
}



void resizeWindow(int w, int h)
{

	winWidth = w;
	winHeight = h;

	xBox = w/4;
	yBox = h/2;

	glViewport( 0, 0, w, h );	// View port uses whole window

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, 0, winWidth, winHeight, 0, 0 );

	glClear(GL_COLOR_BUFFER_BIT);
	refreshDrawing();
}


void keyPressedFn(unsigned char key, int x, int y){

	switch (key){
		case 3:  // CTRL+C
			clearWindow();
		break;

		case 15: // CTRL+O
			toggleOverlay();
		break;

		case 13: // RETURN KEY
			executeCommand(msg);
			bzero(msg,msgLen);
			cIdx = 0;
			eraseCommandString();
		break;
		case '=':
		case '+':
			userScale += dUserScale;
			break;
		case '-':
			userScale -= dUserScale;
			if (userScale<1)
				userScale = 1;
			break;
		case 8: // DEL Key is pressed
			if (cIdx<=0) //if the command string is empty ignore the keypress
				return;
			msg[--cIdx] = 0; //backup the cursor and set the current char to 0
			eraseCommandString();
			break;

		default:
			if(key<' ') // if not a valid Alpha Numeric Char ignore it
				return;
			msg[cIdx++] = key;
	}
}
void dispCommandString(){
	if (cIdx>0)
	{
		
		setViewportForCommandString();
		//Draw a black rectangle to cover up whatever was previously visible
		glColor3f(0.0, 0.0, 0.0);
		glRectf(-1, -1, 2, 2);
		//Draw the white bounding box
		glColor3f(1.0, 1.0, 1.0);
		drawViewportEdge();
		//draw the actuall string
		drawString(-.99,-.35, (char*)msg);
	}
}

void drawString(float x, float y, char *string){

	glRasterPos2f(x, y);

	int len = strlen(string);
	for (int i = 0; i < len; i++) {
    	glutBitmapCharacter(font, string[i]);
	}
}

bool executeCommand(unsigned char *cmd){
	int len = sizeof(cmd);
	std::cout<<"Executing command:"<<cmd<<std::endl;
}

void clearWindow(){
	glClear(GL_COLOR_BUFFER_BIT);
}

void toggleOverlay(){
	clearWave = !clearWave;
}

float scaleVoltage(int v){
	return ((float)v * dV  * userScale) + scaleShift;
}



