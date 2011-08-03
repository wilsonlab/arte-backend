#if defined(__linux__)
	#include <GL/glut.h>
#else // assume OS X
	#include <GLUT/glut.h>
#endif

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include "netcom.h"
#include "datapacket.h"
#include <math.h>
#include <vector>
#include <pthread.h>

#define X_TO_PIX(x) ((x*winWidth)/MAX_INT)
#define PIX_TO_X(p) ((p*MAX_INT)/winWidth)

// these ofsampleRateets create padding between the different plots
// making a more pleasing display

const double MAX_VOLT = pow(2,15);
const int MAX_INT = pow(2,16);
// ===================================
// 		GUI Specific Variables
// ===================================

static int winWidth = 1560, winHeight = 540;
static double commandWinHeight = 20;

static double xBox = 75;
static double yBox = winHeight/8;

static double xPadding = 1;
static double yPadding = 2;

static double const waveformLineWidth = 1;
static bool disableWaveOverlay = true;
static char txtDispBuff[40];

void *font = GLUT_BITMAP_8_BY_13;
static int IDLE_SLEEP_USEC = (1e6)/60;
static int NET_SLEEP_USEC = (1e6)/1000;


// ===================================
// 		Scaling Variables
// ===================================
static int xRange = pow(2,16);
static int yRange = pow(2,16);
static double xScale = 2.0 / xRange;
static double yScale = 2.0 / (double)(yRange);

// Defines how much to shift the waveform within the viewport
//static float dV = 1.0/((float)MAX_VOLT*2);

static float dV = yRange/8;
static float userScale = 1;
static float dUserScale = .1;

static float voltShift = 0;
static float userShift = 0;
static float dUserShift = .01 * pow(2,16);

static float const colSelected[3] = {0.4, 0.4, 0.4};
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
static int nChans=8;

static int const lfpBufsampleRateize = 500;
static lfp_bank_net_t lfpBuff[lfpBufsampleRateize];
static lfp_bank_net_t lfp;
static int nBuff = 0;
static uint64_t readInd = 0;
static uint64_t writeInd = 0;




// ===================================
// 		Plotting Variables
// ===================================
static uint32_t curSeqNum = 0;
static uint32_t prevSeqNum = -1;
static int nSampsPerChan = 2;
static double sampleRate = 2000;
static double winDt = 2;
static int maxIdx = winDt * sampleRate;

static uint64_t dIdx = 0;
//static uint64_t pIdx = 0;


static int xPos = 0;
//static int dXPos = xRange/(maxIdx);

static const int MAX_N_CHAN = 32;
static const int MAX_N_SAMP = 32000;
static GLint waves[MAX_N_CHAN][MAX_N_SAMP*2]; //we need an x and y point for each sample
static int colWave[MAX_N_CHAN];

// ===================================
// 		Inline Functions
// ===================================

int inline IND(uint64_t i)	{	return i%lfpBufsampleRateize;	}
int inline IDX(uint64_t i)	{	return i%maxIdx;	}

inline GLint SCALE_VOLTAGE(int v, int chan){	
	return v * userScale / nChans + yRange - yRange/2/nChans - chan*yRange/nChans  + userShift;
}


// ===================================
// 		Misc Variables
// ===================================
static int totalBufsampleRateRead =0;
static timeval startTime, now;

static int const cmdStrLen = 50;
static char cmd[cmdStrLen];
static int cmdStrIdx = 0;

// ===================================
// 		Command Variables
// ===================================
// Simple commands that respond immediately
static int const CMD_CLEAR_WIN = 'c';
static int const CMD_TOGGLE_OVERLAY = 'o';
static int const CMD_SCALE_UP = '=';
static int const CMD_SCALE_DOWN = '-';
static int const CMD_SHIFT_UP = '+';
static int const CMD_SHIFT_DOWN = '_';
static int const CMD_RESET_SCALE = 'r';
static int const CMD_PREV_COL = '[';
static int const CMD_NEXT_COL = ']';

// Commands that require CTRL + KEY
static int const CMD_QUIT = 17;
static int const CMD_RESET_SEQ = 18;

// Commands that require an input string
static int const CMD_STR_MAX_LEN = 16; 
static int const CMD_GAIN_ALL = 'g';
static int const CMD_GAIN_SINGLE = 'G';
static int const CMD_NULL = 0;
static int const CMD_CHANGE_WIN_LEN = 'L';

static int const KEY_DEL = 127;
static int const KEY_BKSP = 8;
static int const KEY_ENTER = 13;

static int currentCommand = 0;
static bool enteringCommand = false;
static int selectedWaveform = 0;

// ===================================
// 		General Functions
// ===================================
int incrementIdx(int i);
bool tryToGetLfp(lfp_bank_net_t *s);
void updateNChans(int n);
void updateNSamps(int n);
void updateWaveArray();

void idleFn();
void refreshDrawing();
void drawInfoBox();
void eraseOldWaveform();
void clearWaveforms();
void eraseCommandString();

void setViewportforWaveInfoN(int n);
void setViewportForWaves();
void setViewportForCommandString();
void setWaveformColor(int col);

void drawViewportEdge();

void drawWaveforms();
void drawWaveformN(int n);

void setWaveformColor(int c);
void highlightSelectedWaveform();

void resizeWindow(int w, int h);

int scaleVoltage(int v, int chan);
// ===================================
// 		Keyboard & Command Function Headers
// ===================================

void toggleOverlay();
void clearWindow();

void keyPressedFn(unsigned char key, int x, int y);
void specialKeyFn(int key, int x, int y);

void enterCommandStr(char key);
void dispCommandString();

bool executeCommand(char * cmd);
void updateWinLen(double l);

void drawString(float x, float y, char *string);
void *readNetworkLfpData(void *ptr);

void initializeWaveVariables();
void loadWaveformColors();

int main( int argc, char** argv )
{
	if (argc>1)
		port = argv[1];
	else
	{
		std::cout<<"Usage: artelfpViewer port"<<std::endl;
		return 0;
	}

	std::cout<<"================================================"<<std::endl;
	std::cout<<" Starting up Arte Lfp Viewer"<<std::endl;
	std::cout<<"================================================"<<std::endl;


	initializeWaveVariables();
	loadWaveformColors();
	
	pthread_t netThread;
	net = NetCom::initUdpRx(host,port);
	pthread_create(&netThread, NULL, readNetworkLfpData, (void *)NULL);
	
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowPosition( 5, 20 );
	glutInitWindowSize( winWidth, winHeight);

	char windowTitle[100];
	sprintf(windowTitle, "Arte lfp Viewer: (port)");
	glutCreateWindow(windowTitle);
	
	glutReshapeFunc( resizeWindow );
	glutIdleFunc( idleFn );
	glutDisplayFunc( refreshDrawing );
	
	glutKeyboardFunc(keyPressedFn);
	glutSpecialFunc(specialKeyFn);

	//std::cout<<"Starting the GLUT main Loop"<<std::endl;
	glutMainLoop(  );

	return(0);
}
void initializeWaveVariables(){
	
	sampleRate = 1000 * nSampsPerChan;		
	maxIdx = winDt * sampleRate;
	
	if (maxIdx>MAX_N_SAMP){
		std::cout<<"specified time window is too long, closing -1"<<std::endl;
		std::cout<<"Restrict the SampleRate * WinLen to less than: "<<maxIdx<<std::endl;
		exit(-1);
	}

	std::cout<<"Clearing the existing waveform data"<<std::endl;
	bzero(waves, MAX_N_CHAN*MAX_N_SAMP*2);
	for (int i=0; i<MAX_N_CHAN; i++)
	{
		for (int j=0; j<maxIdx; j++)
		{
			waves[i][j*2] = j * xRange / maxIdx;
			waves[i][j*2+1] = 0;
		}
	}	
				
	bzero(cmd, cmdStrLen);
	readInd = 0;
	writeInd = 0;
}

void loadWaveformColors(){
	for (int i=0; i<MAX_N_CHAN; i++)
		colWave[i] = i;
}

void *readNetworkLfpData(void *ptr){
	
	lfp_bank_net_t l;

	while(true)
	{
		NetCom::rxWave(net, &l);
		lfpBuff[IND(writeInd)] = l;
		writeInd++;
	}
}

bool tryToGetLfp(lfp_bank_net_t *l){

	if  ( readInd >= writeInd )
		return false;

	l->name 	= lfpBuff[IND(readInd)].name;
	l->n_chans 	= lfpBuff[IND(readInd)].n_chans;
	l->n_samps_per_chan = lfpBuff[IND(readInd)].n_samps_per_chan;
	l->samp_n_bytes 	= lfpBuff[IND(readInd)].samp_n_bytes;
	for (int i=0; i < lfpBuff[IND(readInd)].n_chans *  lfpBuff[IND(readInd)].n_samps_per_chan; i++)
		l->data[i] = lfpBuff[IND(readInd)].data[i];

	for (int i=0; i < lfpBuff[IND(readInd)].n_chans; i++){
		l->gains[i] = lfpBuff[IND(readInd)].gains[i];
	}

	l->seq_num = lfpBuff[IND(readInd)].seq_num;

	readInd++;

	if (l->n_chans != nChans)
		updateNChans(l->n_chans);

	if (l->n_samps_per_chan != nSampsPerChan)
		updateNSamps(l->n_samps_per_chan);

	return true;
}

void updateNChans(int n){
	std::cout<<"nChans changed to:"<<n<<std::endl;
	nChans = n;
	yBox = (double)winHeight/nChans;
	
	curSeqNum = 0;

	initializeWaveVariables();
	clearWaveforms();
	clearWindow();

}
void updateNSamps(int n){
	std::cout<<"nSamps changed to:"<<n<<std::endl;
	nSampsPerChan = n;
//	sampleRate = 1000 * nSampsPerChan;
//	maxIdx = winDt * sampleRate;

	curSeqNum = 0;

	initializeWaveVariables();
	clearWaveforms();
	clearWindow();

}

void updateWaveArray(){
	prevSeqNum = curSeqNum;

	// if we receive an old packet ignore it
	if (lfp.seq_num < curSeqNum) 
	{
		std::cout<<"Old packet received, ignoring it! ";//<<lfp.seq_num<<" curSeqNum"<<curSeqNum<<std::endl;
		std::cout<<"Press CTRL+R to reset sequence number:"<<std::endl;
		return;
	}
	
	else if(lfp.seq_num == curSeqNum) // if we have the same packet as last time
		return;

	int i=0, j=0, k = 0;
	
	for (i=0; i<lfp.n_samps_per_chan; i++)
	{
		for (j=0; j<lfp.n_chans; j++)
			waves[j][IDX(dIdx)*2+1] = SCALE_VOLTAGE(lfp.data[k++],j);
		dIdx++;
	}
	xPos = waves[0][IDX(dIdx)*2];
	curSeqNum = lfp.seq_num;
}


void idleFn(void){
	do{
		updateWaveArray();
	}while(tryToGetLfp(&lfp));
	
	refreshDrawing();
    usleep(IDLE_SLEEP_USEC);
//	std::cout<<"idleFn Sleeping"<<std::endl;
}

void refreshDrawing(void)
{
	glViewport(0, 0, winWidth, winHeight);
	glLoadIdentity ();

	drawWaveforms();

	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	drawViewportEdge();

	drawInfoBox();

	dispCommandString();

	glutSwapBuffers();
	glFlush();
}

void eraseOldWaveform(){

	int pixWidth = xRange/winWidth;

	int x = xPos;
	
	int dx = (waves[0][2] - waves[0][0])*sampleRate/20;


	glColor3f(0.0, 0.0, 0.0);

	glRecti(x-dx/2, PIX_TO_X(0), x+dx/2, yRange);
 
	// EDGE CASES
	// If erase box is clipped on the left, draw it again but shifted all the way to the right and visa versa
	if( (x-dx/2) < 0 )
		glRecti(x - dx/2 + xRange, PIX_TO_X(0), x + dx/2 + xRange, yRange);
	else if( (x+dx/2) > xRange)
		glRecti(x - dx/2 - xRange, PIX_TO_X(0), x + dx/2 - xRange, yRange);



}
void clearWaveforms(){

	std::cout<<"Clearing all waveforms"<<std::endl;
	glViewport( xPadding, 0, winWidth-xPadding, winHeight );	// View port uses whole window

	glColor3f(0,0,0);
	glRectf(-1, -1, 2, 2);
	
	glLineWidth(1);
	glColor3f(1.0, 1.0, 1.0);

	drawViewportEdge();

}

void eraseCommandString(){
	setViewportForCommandString();
	glColor3f(0,0,0);
	glRectf(-1, -1, 2, 2);
}


void drawWaveforms(void){

	setViewportForWaves();

	glLineWidth(waveformLineWidth);
	
	glEnableClientState(GL_VERTEX_ARRAY);

	for (int i=0; i<lfp.n_chans; i++) 
			drawWaveformN(i);

	glDisableClientState(GL_VERTEX_ARRAY);
	
	if (disableWaveOverlay)
		eraseOldWaveform();
		
	glLoadIdentity();

}


void drawWaveformN(int n){

	setWaveformColor(colWave[n]);

	glVertexPointer(2, GL_INT, 0, waves[n]);

	glDrawArrays(GL_LINE_STRIP, 0, maxIdx);

					
}

void setViewportForWaveInfoN(int n){
  float viewDx = xBox;
  float viewDy = yBox+.5;
  float viewX = xPadding;
  float viewY = (nChans - (n+1)) * yBox;

  glViewport(viewX, viewY, viewDx, viewDy);

}
void setViewportForWaves(){

	glViewport( xBox+xPadding, 0, winWidth-xBox-xPadding, winHeight-yPadding );	// View port uses whole window
	glLoadIdentity ();
	glTranslatef(-1.0, -1.0, 0.0);
	glScalef(xScale, yScale, 0);

}

void setViewportForCommandString(){

    float viewX = 0 + xPadding;
    float viewY = 0 + yPadding;
    float viewDX = xBox*3;
    float viewDY =  commandWinHeight;

	glViewport(viewX, viewY, viewDX, viewDY);
}

void setWaveformColor(int c){

	int nColor = 13;
	switch(c%nColor){
		case 0: // red
			glColor3f(1.0, 0.0, 0.0);
			break;
		case 1: // red
			glColor3f(1.0, 0.5, 0.0);
			break;
		case 2: // red
			glColor3f(1.0, 1.0, 0.0);
			break;
		case 3: // red
			glColor3f(0.5, 1.0, 0.0);
			break;
		case 4: // white
			glColor3f(0.0, 1.0, 0.0);
			break;
		case 5: // green
			glColor3f(0.0, 1.0, 0.5);
			break;
		case 6: //
			glColor3f(0.0, 1.0, 1.0);
			break;
		case 7:
			glColor3f(0.0, 0.5, 1.0);
			break;
		case 8:
			glColor3f(0.0, 0.0, 1.0);
			break;
		case 9:
			glColor3f(0.5, 0.0, 1.0);
			break;
		case 10:
			glColor3f(1.0, 0.0, 1.0);
			break;
		case 11:
			glColor3f(1.0, 0.0, 0.5);
			break;
		default:
			glColor3f(1.0, 1.0, 1.0);
	}	
}

void drawInfoBox(void){

	char txt[20];
	bzero(txt,20);
    glLineWidth(1.0);
    for (int i=0; i<nChans; i++){
    	setViewportForWaveInfoN(i);
	  	if (i==selectedWaveform)
			glColor3f(.25, .25, .25);
		else 
			glColor3f(0, 0, 0);
		glRectf(-1, -1, 2, 2);
		
		glColor3f(1.0, 1.0, 1.0);
    	drawViewportEdge();

		setWaveformColor(colWave[i]);
		sprintf(txt, "T:18.%d", i);
		drawString(-.7, -.2, txt);
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

	yBox = (double)h/(double)nChans;

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
      case GLUT_KEY_UP: // up
        selectedWaveform -=1;
        break;
      case GLUT_KEY_DOWN: //down
        selectedWaveform +=1;
        break;
    }
    if (selectedWaveform<0)
        selectedWaveform +=nChans;
    if (selectedWaveform>=nChans)
        selectedWaveform -=nChans;
	std::cout<<"SelectedWaveform:"<<selectedWaveform<<std::endl;
}

void keyPressedFn(unsigned char key, int x, int y){

	std::cout<<"Key Pressed:"<<key<<" value:"<<(int)key<<std::endl;

	if (enteringCommand){
		enterCommandStr(key);
		return;
	}
	switch (key){
		case CMD_QUIT:
			exit(1);
			break;
		case CMD_RESET_SEQ:
			curSeqNum = 0;
			clearWindow();
			std::cout<<"Reseting sequence number to 0!"<<std::endl;
			break;
		case CMD_CLEAR_WIN:
			clearWaveforms();
			clearWindow();
			break;
		case CMD_SCALE_UP:
			userScale += dUserScale;
			break;
		case CMD_SCALE_DOWN:
			userScale -= dUserScale;
			if (userScale<1)
				userScale = 1;
			break;
		case CMD_SHIFT_UP:
			userShift += dUserShift;
			break;
		case CMD_SHIFT_DOWN:
			userShift -= dUserShift;
			break;		
		case CMD_RESET_SCALE:
			userShift = 0;
			userScale = 1;
			clearWindow();
			break;
		case CMD_PREV_COL:
			colWave[selectedWaveform]--;
			std::cout<<"Wave:"<<selectedWaveform<<" set to color:"<<colWave[selectedWaveform]<<std::endl;
			break;
		case CMD_NEXT_COL:
			colWave[selectedWaveform]++;
			std::cout<<"Wave:"<<selectedWaveform<<" set to color:"<<colWave[selectedWaveform]<<std::endl;
			break;
		// -------------------------------------------
		// Commands that require additional user input
		// -------------------------------------------
		case CMD_GAIN_ALL:
		case CMD_CHANGE_WIN_LEN:
			enteringCommand = true;
			currentCommand = key;
			break;
		}

 }

void enterCommandStr(char key){
	std::cout<<"Entering command";
	switch(key){
		// Erase command string
		case KEY_BKSP: //  Backspace Key
		case KEY_DEL: // MAC Delete key is pressed
			if (cmdStrIdx<=0){ //if the command string is empty ignore the keypress
				enteringCommand = false;
				return;
			}
			cmd[--cmdStrIdx] = 0; //backup the cursor and set the current char to 0
			eraseCommandString();
			break;
		case KEY_ENTER: // RETURN KEY
			executeCommand((char*)cmd);
			bzero(cmd,cmdStrLen);
			cmdStrIdx = 0;
			eraseCommandString();
			enteringCommand = false;
			refreshDrawing(); // to erase the command window if no lfps are coming in
		break;
		default:
			if(key<' ') // if not a valid Alpha Numeric Char ignore it
				return;
			cmd[cmdStrIdx] = key;
			if (cmdStrIdx<CMD_STR_MAX_LEN)
				cmdStrIdx+=1;
			std::cout<<cmdStrIdx<<std::endl;
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
		for (int i=0; i<cmdStrIdx; i++)
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

bool executeCommand(char *cmd){
	int len = sizeof(cmd);
	std::cout<<"Executing command:"<<cmd<<std::endl;

	switch(currentCommand){
		case CMD_CHANGE_WIN_LEN:
			std::cout<<"Changing window len to:"<<cmd<<std::endl;
			double l = atof(cmd);
			updateWinLen(l);
		break;

	}
	currentCommand  = CMD_NULL;
}
void updateWinLen(double l){
			if (sampleRate * l > MAX_N_SAMP)
				std::cout<<"Invalid window length specified! The requested window would require:"<<sampleRate * l<<" samples. However \
							the number of samples cannot exceed:"<<MAX_N_SAMP<<std::endl;
			else if (l<=0)
				std::cout<<"Invalid window length specified. Please pick a positive number"<<std::endl;
			else if ( l==winDt)
			{}
			else
			{
				winDt = l;
				initializeWaveVariables();
				clearWindow();
			}
}

void clearWindow(){
	initializeWaveVariables();
	glClear(GL_COLOR_BUFFER_BIT);
	dIdx = 0;
	xPos = 0;
}

void toggleOverlay(){
	disableWaveOverlay = !disableWaveOverlay;
	if(disableWaveOverlay)
		clearWindow();
}

int incrementIdx(int i){
	if(i==lfpBufsampleRateize-1)
		return 0;
	else
		return i+1;
}

