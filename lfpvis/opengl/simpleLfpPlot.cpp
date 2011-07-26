#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GLUT/glut.h>	// OpenGL Graphics Utility Library
#include "netcom.h"
#include "datapacket.h"
#include <math.h>
#include <vector>
#include <pthread.h>

// these offsets create padding between the different plots
// making a more pleasing display

const double MAX_VOLT = pow(2,15);

// ===================================
// 		GUI Specific Variables
// ===================================
static double winWidth = 624, winHeight = 312;
static double commandWinHeight = 20; // Shift the entire UI up by commandWinHeight and reserve this area for text and buttons

static double xBox = winWidth;
static double yBox = winHeight/8;

static double xPadding = 2;
static double yPadding = 2;

static double const waveformLineWidth = 1;
static bool disableWaveOverlay = true;
static char txtDispBuff[40];

void *font = GLUT_BITMAP_8_BY_13;
static int TIMEOUT = (1e6)/500;

// Scaling Variables
// Defines how much to shift the waveform within the viewport
static float dV = 1.0/((float)MAX_VOLT*2);
static float userScale = 1;
static float dUserScale = .3;

static float voltShift = -.85;
static float userShift = 0;
static float dUserShift = .05;
// ===================================
// 		Network Variables
// ===================================

static char host[] = "127.0.0.1";
static char * port;
static NetComDat net; // = NetCom::initUdpRx(host,port);


// ===================================
// 		Data Variables
// ===================================
static int nChan=8;

static int const lfpBuffSize = 500;
static lfp_bank_net_t lfpBuff[lfpBuffSize];
static lfp_bank_net_t lfp;
static int nBuff = 0;
static int readIdx = 0;
static int writeIdx = 0;

// ===================================
// 		Misc Variables
// ===================================
static int totalBuffsRead =0;
static timeval startTime, now;

static int const cmdStrLen = 50;
static unsigned char cmd[cmdStrLen];
static int cIdx = 0;

// ===================================
// 		Command Variables
// ===================================
static int const CMD_CLEAR_WIN = 'c';
static int const CMD_TOGGLE_OVERLAY = 'o';
static int const CMD_SCALE_UP = '=';
static int const CMD_SCALE_DOWN = '-';
static int const CMD_SHIFT_UP = '+';
static int const CMD_SHIFT_DOWN = '_';

static int const CMD_STR_MAX_LEN = 16;
static int const CMD_GAIN_ALL = 'g';
static int const CMD_GAIN_SINGLE = 'G';
static int const CMD_NULL = 0;
static int currentCommand = 0;
static bool enteringCommand = false;

// ===================================
// 		General Functions
// ===================================
int incrementIdx(int i);
bool tryToGetlfp(lfp_bank_net_t *s);

void idleFn();
void refreshDrawing();
void drawBoundingBox();
void eraseWaveforms();
void eraseCommandString();

void setViewportForWaveN(int n);
void setViewportForCommandString();

void drawViewportEdge();

void drawWaveforms();
void drawWaveformN(int n);

void resizeWindow(int w, int h);

float scaleVoltage(int v, bool);
// ===================================
// 		Keyboard & Command Function Headers
// ===================================

void toggleOverlay();
void clearWindow();

void keyPressedFn(unsigned char key, int x, int y);
void enterCommandStr(char key);
void dispCommandString();

bool executeCommand(unsigned char * cmd);

void drawString(float x, float y, char *string);
void *getNetlfp(void *ptr);


int main( int argc, char** argv )
{
	if (argc>1)
		port = argv[1];
	else
	{
		std::cout<<"Usage: artelfpViewer port"<<std::endl;
		return 0;
	}


	bzero(cmd, cmdStrLen);
	pthread_t netThread;
	net = NetCom::initUdpRx(host,port);
	pthread_create(&netThread, NULL, getNetlfp, (void *)NULL);
	
	srand(time(NULL));
	gettimeofday(&startTime,NULL);


	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
//	glDisable(GL_DEPTH_TEST);
	glutInitWindowPosition( 20, 60 );
	glutInitWindowSize( winWidth, winHeight);

	char windowTitle[100];

	sprintf(windowTitle, "Arte lfp Viewer: (port)");

	glutCreateWindow(windowTitle);
	glutReshapeFunc( resizeWindow );
	glutIdleFunc( idleFn );
	glutDisplayFunc( refreshDrawing );
	glutKeyboardFunc(keyPressedFn);

	std::cout<<"Starting the GLUT main Loop"<<std::endl;
	glutMainLoop(  );

	return(0);
}


bool tryToGetlfp(lfp_bank_net_t *l){

	if  (readIdx==writeIdx || nBuff==0)
		return false;

	// Copy the lfp data
	l->name 	= lfpBuff[readIdx].name;
	l->n_chans 	= lfpBuff[readIdx].n_chans;
	l->n_samps_per_chan = lfpBuff[readIdx].n_samps_per_chan;
	l->samp_n_bytes 	= lfpBuff[readIdx].samp_n_bytes;
	for (int i=0; i < lfpBuff[readIdx].n_chans *  lfpBuff[readIdx].n_samps_per_chan; i++)
		l->data[i] = lfpBuff[readIdx].data[i];

	for (int i=0; i < lfpBuff[readIdx].n_chans; i++){
		l->gains[i] = lfpBuff[readIdx].gains[i];
	}

	readIdx = incrementIdx(readIdx);
	nBuff--;
	
	return true;
}



void *getNetlfp(void *ptr){
	while(true)
	{
		lfp_bank_net_t l;
		NetCom::rxWave(net, &l);
		lfpBuff[writeIdx] = l;
		writeIdx = incrementIdx(writeIdx);
		nBuff+=1;
		totalBuffsRead++;
	}
}


void idleFn(void){
	if (tryToGetlfp(&lfp) || enteringCommand)
		refreshDrawing();
    usleep(TIMEOUT);
}


void refreshDrawing(void)
{
	if (disableWaveOverlay)
		eraseWaveforms();

	drawWaveforms();
	drawBoundingBox();
	dispCommandString();

	glutSwapBuffers();
	glFlush();
}

void eraseWaveforms(){

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
	glRectf(-1, -1, 2, 2);
}


void drawWaveforms(void){

	glLineWidth(waveformLineWidth);
	for (int i=0; i<lfp.n_chans; i++) 
			drawWaveformN(i);
}


void drawWaveformN(int n)
{
	setViewportForWaveN(n);

	// Draw the actual waveform
	float dx = 2.0/(lfp.n_samps_per_chan-1);
	float x = -1;
	int	sampIdx = n; 
	glColor3f(1.0, 1.0, 0.6);
	glBegin( GL_LINE_STRIP );
		for (int i=0; i<lfp.n_samps_per_chan; i++)
		{
			glVertex2f(x, scaleVoltage(lfp.data[sampIdx], true));
			sampIdx +=4;
			x +=dx;
		}
	glEnd();

	// Draw the waveform marker line
	glColor3f(1.0, 0.0, 0.0); // set threshold line to red
	glBegin( GL_LINE_STRIP );
		glVertex2f(-1.0, 0);
		glVertex2f( 0, 0);
	glEnd();		
}


void setViewportForWaveN(int n){
	float viewDX = winWidth - 2*xPadding;
	float viewDY = yBox - 2*yPadding;
	float viewX = 0 + xPadding;
	float viewY = (nChan - (n+1)) * yBox  + yPadding;   
    
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
    float viewDX = xBox - 2*xPadding;
    float viewDY =  commandWinHeight;

	glViewport(viewX, viewY, viewDX, viewDY);
}

void drawBoundingBox(void){

	glColor3f(1.0, 1.0, 1.0);
	glViewport( 0, 0, winWidth, winHeight );	// View port uses whole window
	drawViewportEdge();
	
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

	yBox = h/nChan;

	glViewport( 0, 0, w, h );	// View port uses whole window

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, 0, winWidth, winHeight, 0, 0 );

	glClear(GL_COLOR_BUFFER_BIT);
	refreshDrawing();
}


void keyPressedFn(unsigned char key, int x, int y){

	std::cout<<"Key Pressed:"<<key<<" value:"<<(int)key<<std::endl;

	if (enteringCommand){
		enterCommandStr(key);
		return;
	}
	switch (key){
		case CMD_CLEAR_WIN:
			clearWindow();
		break;
		case CMD_TOGGLE_OVERLAY: 
			toggleOverlay();
		break;
		// Scale Waveforms and Projections
		case CMD_SCALE_UP:
			userScale += dUserScale;
			break;
		case CMD_SCALE_DOWN:
			userScale -= dUserScale;
			if (userScale<1)
				userScale = 1;
			break;
		// Shift the waveforms only
		case CMD_SHIFT_UP:
			userShift += dUserShift;
			std::cout<<"User shift raised:"<<userShift<<std::endl;
			break;
		case CMD_SHIFT_DOWN:
			userShift -= dUserShift;
			std::cout<<"User shift lowered:"<<userShift<<std::endl;
			break;		
		// Commands that require additional user input
		case CMD_GAIN_ALL:
			enteringCommand = true;
			currentCommand = key;
			break;
		}
 }
void enterCommandStr(char key){
	switch(key){
		// Erase command string
		case 8: //  Backspace Key
		case 127: // MAC Delete key is pressed
			if (cIdx<=0){ //if the command string is empty ignore the keypress
				enteringCommand = false;
				return;
			}
			cmd[--cIdx] = 0; //backup the cursor and set the current char to 0
			eraseCommandString();
			break;
		case 13: // RETURN KEY
			executeCommand(cmd);
			bzero(cmd,cmdStrLen);
			cIdx = 0;
			eraseCommandString();
			enteringCommand = false;
			refreshDrawing(); // to erase the command window if no lfps are coming in
		break;
		default:
			if(key<' ') // if not a valid Alpha Numeric Char ignore it
				return;
			cmd[cIdx] = key;
			if (cIdx<CMD_STR_MAX_LEN)
				cIdx+=1;
				std::cout<<cIdx<<std::endl;
			std::cout<<"Command Entered:"<<cmd<<std::endl;
	}
}

void dispCommandString(){
	if (enteringCommand)
	{
		setViewportForCommandString();
		//Draw a black rectangle to cover up whatever was previously visible
		glColor3f(0.0, 0.0, 0.0);
		glRectf(-1, -1, 2, 2);
		//Draw the white bounding box
		glColor3f(1.0, 1.0, 1.0);
		drawViewportEdge();

		// Prepend the command char and :  for display purposes
		char dispCmd[CMD_STR_MAX_LEN+3];
		bzero(dispCmd, CMD_STR_MAX_LEN+3);
		for (int i=0; i<cIdx; i++)
			dispCmd[i+2] = cmd[i];
		dispCmd[0] = currentCommand;
		dispCmd[1] = ':';
		//draw the actuall string
		drawString(-.95,-.35, (char*)dispCmd);
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
	currentCommand  = CMD_NULL;
}

void clearWindow(){
	glClear(GL_COLOR_BUFFER_BIT);
}

void toggleOverlay(){
	disableWaveOverlay = !disableWaveOverlay;
}

float scaleVoltage(int v, bool shift){
	if (shift)
		return ((float)v * dV * userScale) + voltShift + userShift;
	else
		return ((float)v * dV * userScale) + voltShift;
}

int incrementIdx(int i){
	if(i==lfpBuffSize-1)
		return 0;
	else
		return i+1;
}
