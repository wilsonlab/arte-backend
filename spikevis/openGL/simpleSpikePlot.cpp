#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

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

static double xBox = winWidth/4;
static double yBox = winHeight/2;

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

static float const colWave[3] = {1.0, 1.0, 0.6};
static float const colThres[3] = {1.0, 0.1, 0.1};
static float const colSelected[3] = {1.0, 1.0, 1.0};
static float const colFont[3] = {1.0, 1.0, 1.0};
// ===================================
// 		Network Variables
// ===================================

static char host[] = "127.0.0.1";
static char * port;
static NetComDat net; // = NetCom::initUdpRx(host,port);

// ===================================
// 		Data Variables
// ===================================
static spike_net_t spikeOld;
static int nChan=4;
static int nProj=6;

static int const spikeBuffSize = 500;
static spike_net_t spikeBuff[spikeBuffSize];
static spike_net_t spike;
static int nSpikes = 0;
static int readIdx = 0;
static int writeIdx = 0;

// ===================================
// 		Misc Variables
// ===================================
static int totalSpikesRead =0;
static timeval startTime, now;

static int const cmdStrLen = 50;
static unsigned char cmd[cmdStrLen];
static int cIdx = 0;
// ===================================
// 		
// ===================================

void initNetworkRxThread();
void checkForNewSpikes();
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
static int const CMD_THOLD_ALL = 'T';
static int const CMD_THOLD_SINGLE = 't';
static int const CMD_GAIN_ALL = 'G';
static int const CMD_GAIN_SINGLE = 'g';
static int const CMD_SET_POST_SAMPS = 'N';
static int const CMD_NULL = 0;
static int currentCommand = 0;
static bool enteringCommand = false;
static int selectedWaveform = 0;

// ===================================
// 		General Functions
// ===================================
int incrementIdx(int i);
bool tryToGetSpike(spike_net_t *s);

void idleFn();
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
void drawSelectedWaveform();

void drawProjections();
void drawProjectionN(int n, int idx);
int calcWaveMaxInd();

void resizeWindow(int w, int h);

float scaleVoltage(int v, bool);
// ===================================
// 		Keyboard & Command Function Headers
// ===================================

void toggleOverlay();
void clearWindow();

void keyPressedFn(unsigned char key, int x, int y);
void specialKeyFn(int key, int x, int y);
void enterCommandStr(char key);
void dispCommandString();

bool executeCommand(unsigned char * cmd);

void drawString(float x, float y, char *string);
void *getNetSpike(void *ptr);



int main( int argc, char** argv )
{
	if (argc>1)
		port = argv[1];
	else
	{
		std::cout<<"Usage: arteSpikeViewer port"<<std::endl;
		return 0;
	}


	bzero(cmd, cmdStrLen);
	pthread_t netThread;
	net = NetCom::initUdpRx(host,port);
	pthread_create(&netThread, NULL, getNetSpike, (void *)NULL);
	
	srand(time(NULL));
	gettimeofday(&startTime,NULL);


	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
//	glDisable(GL_DEPTH_TEST);
	glutInitWindowPosition( 20, 60 );
	glutInitWindowSize( winWidth, winHeight);

	char windowTitle[100];

	sprintf(windowTitle, "Arte Spike Viewer: (port)");

	glutCreateWindow(windowTitle);
	glutReshapeFunc( resizeWindow );
//	glutIdleFunc( getNetSpike );
	glutIdleFunc( idleFn );
	glutDisplayFunc( refreshDrawing );
	glutKeyboardFunc(keyPressedFn);
	glutSpecialFunc(specialKeyFn);

	std::cout<<"Starting the GLUT main Loop"<<std::endl;
	glutMainLoop(  );

	return(0);
}


bool tryToGetSpike(spike_net_t *s){

	if  (readIdx==writeIdx || nSpikes==0)
		return false;

	// Copy the spike data
	s->name 	= spikeBuff[readIdx].name;
	s->n_chans 	= spikeBuff[readIdx].n_chans;
	s->n_samps_per_chan = spikeBuff[readIdx].n_samps_per_chan;
	s->samp_n_bytes 	= spikeBuff[readIdx].samp_n_bytes;
	for (int i=0; i < spikeBuff[readIdx].n_chans *  spikeBuff[readIdx].n_samps_per_chan; i++)
		s->data[i] = spikeBuff[readIdx].data[i];

	for (int i=0; i < spikeBuff[readIdx].n_chans; i++){
		s->gains[i] = spikeBuff[readIdx].gains[i];
		s->thresh[i]= spikeBuff[readIdx].thresh[i];
	}

	readIdx = incrementIdx(readIdx);
	nSpikes--;
	
	return true;
}



void *getNetSpike(void *ptr){
	while(true)
	{
		spike_net_t s;
		NetCom::rxSpike(net, &s);
		spikeBuff[writeIdx] = s;
		writeIdx = incrementIdx(writeIdx);
		nSpikes+=1;
		totalSpikesRead++;
	}
}


void idleFn(void){
	if (tryToGetSpike(&spike) || enteringCommand)
		refreshDrawing();
    usleep(TIMEOUT);
}


void refreshDrawing(void)
{
	if (disableWaveOverlay)
		eraseWaveforms();

	drawWaveforms();
	drawProjections();
	drawBoundingBoxes();
 	//drawSelectedWaveform();
	dispCommandString();

	glutSwapBuffers();
	glFlush();
}

void eraseWaveforms(){

	for (int i=0; i<nChan; i++)
	{
		setViewportForWaveN(i);
		if (i==selectedWaveform)
			glColor3f(.15,.15,.15);
		else	
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
	for (int i=0; i<spike.n_chans; i++) 
			drawWaveformN(i);
}


void drawWaveformN(int n)
{
	setViewportForWaveN(n);

	// Disp the threshold value
	int thresh = spike.thresh[n];
	sprintf(txtDispBuff, "T:%d", thresh);
	glColor3f(colFont[0],colFont[1],colFont[2]);
	drawString(-.9, .8, txtDispBuff);

	// Draw the actual waveform
	float dx = 2.0/(spike.n_samps_per_chan-1);
	float x = -1;
	int	sampIdx = n; 
	glColor3f(colWave[0], colWave[1], colWave[2]);
	glBegin( GL_LINE_STRIP );
		for (int i=0; i<spike.n_samps_per_chan; i++)
		{
			glVertex2f(x, scaleVoltage(spike.data[sampIdx], true));
			sampIdx +=4;
			x +=dx;
		}
	glEnd();

	// Draw the threshold line
	glColor3f(colThres[0], colThres[1], colThres[2]); // set threshold line to red
	glLineStipple(4, 0xAAAA); // make line a dashed line
	glEnable(GL_LINE_STIPPLE);
	glBegin( GL_LINE_STRIP );
		glVertex2f(-1.0, scaleVoltage(thresh, true));
		glVertex2f( 1.0, scaleVoltage(thresh, true));
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



void drawProjections(){

	int maxIdx = calcWaveMaxInd();

	maxIdx = maxIdx  - maxIdx%4;  

	for (int i=0; i<nProj; i++) // <----------------------------------------
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
	glBegin(GL_POINTS);
		glVertex2f(scaleVoltage(spike.data[idx+d1], false), scaleVoltage(spike.data[idx+d2], false));
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

void drawSelectedWaveform(){
  setViewportForWaveN(selectedWaveform);
  glLineWidth(2);
  glColor3f(colSelected[0], colSelected[1], colSelected[2]);
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

	xBox = w/4;
	yBox = h/2;

	glViewport( 0, 0, w, h );	// View port uses whole window

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, 0, winWidth, winHeight, 0, 0 );

	glClear(GL_COLOR_BUFFER_BIT);
	refreshDrawing();
}

void specialKeyFn(int key, int x, int y){
	std::cout<<"Key Pressed:"<<key<<std::endl;
    switch(key){
      case GLUT_KEY_LEFT: // left
        selectedWaveform -=1;
        break;
      case GLUT_KEY_UP: // up
        selectedWaveform -=2;
        break;
      case GLUT_KEY_RIGHT: // right
        selectedWaveform +=1;
        break;
      case GLUT_KEY_DOWN: //down
        selectedWaveform +=2;
        break;
    }
    if (selectedWaveform<0)
        selectedWaveform +=4;
    if (selectedWaveform>=4)
        selectedWaveform -=4;
            
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
		case CMD_GAIN_SINGLE:
		case CMD_THOLD_SINGLE:
		case CMD_THOLD_ALL:
		case CMD_SET_POST_SAMPS:
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
				eraseCommandString();
				currentCommand = NULL;
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
			refreshDrawing(); // to erase the command window if no spikes are coming in
		break;
		default:
			if(key<' ') // if not a valid Alpha Numeric Char ignore it
				return;
			cmd[cIdx] = key;
			if (cIdx<CMD_STR_MAX_LEN)
				cIdx+=1;
				std::cout<<cIdx<<std::endl;
//			std::cout<<"Command Entered:"<<cmd<<std::endl;
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
	switch(currentCommand){
		case CMD_THOLD_ALL:
		std::cout<<"Changing all thresholds!"<<std::endl;
		break;
		case CMD_THOLD_SINGLE:
		std::cout<<"Changing a single threshold!"<<std::endl;
		break;
	}
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
	if(i==spikeBuffSize-1)
		return 0;
	else
		return i+1;
}
