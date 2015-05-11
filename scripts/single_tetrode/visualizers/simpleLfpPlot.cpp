#include "simpleLfpPlot.h"

int main( int argc, char** argv )
{
	
	for (int i=0; i<MAX_N_CHAN; i++)
		setChannelLabel(i, defName, CHAN_NAME_LEN);	
//	memcpy(chName[i], &"HPC N:M", CHAN_NAME_LEN);

	initCommandListAndMap();
	parseCommandLineArgs(argc, argv);

	if(strlen(windowTitle)==0)
		memcpy(windowTitle, &"Arte Network LFP Viewer", 23);
	
/*
	if (argc>1)
		port = argv[1];
	else
	{
		std::cout<<"Usage: artelfpViewer port"<<std::endl;
		return 0;
	}*/

	std::cout<<"================================================"<<std::endl;
	std::cout<<" Starting up Arte Lfp Viewer"<<std::endl;
	std::cout<<"================================================"<<std::endl;


	initializeWaveVariables();
	loadWaveformColors();
	
	pthread_t netThread;
	net = NetCom::initUdpRx(host,port); //error
	pthread_create(&netThread, NULL, readNetworkLfpData, (void *)NULL);

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowPosition( winPosX, winPosY );
	glutInitWindowSize( winWidth, winHeight);

//	sprintf(windowTitle, "Arte lfp Viewer: (port)");
	glutCreateWindow(windowTitle);

	glutReshapeFunc( resizeWindow );
	glutIdleFunc( idleFn );
	glutDisplayFunc( redrawWindow );
	
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
			waves[i][j*2+1] = SCALE_VOLTAGE(0,i);
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
	//eraseWaveformViewport();
	clearWindow();

}
void updateNSamps(int n){
	std::cout<<"nSamps changed to:"<<n<<std::endl;
	nSampsPerChan = n;
//	sampleRate = 1000 * nSampsPerChan;
//	maxIdx = winDt * sampleRate;

	curSeqNum = 0;

	initializeWaveVariables();
	//eraseWaveformViewport();
	clearWindow();

}

void updateWaveArray(){
	prevSeqNum = curSeqNum;
	
	int i=0, j=0, k = 0;
	
	if(curSeqNum==0 && lfp.seq_num>0)
		curSeqNum = lfp.seq_num-1;
	
	if (lfp.seq_num <= curSeqNum)  // If this packet is old ignore it
		return;
	
	else if(lfp.seq_num == curSeqNum+1) // if we have the NEXT packet in the sequence
		for (i=0; i<lfp.n_samps_per_chan; i++)
		{
			for (j=0; j<lfp.n_chans; j++)
				waves[j][IDX(dIdx)*2+1] = SCALE_VOLTAGE(lfp.data[k++],j);
			dIdx++;
		}

	else if(lfp.seq_num > curSeqNum+1)
	{
		while(lfp.seq_num > curSeqNum+1) // if the packet is not the next packet (ie packets got dropped)
		{
			for (i=0; i<lfp.n_samps_per_chan; i++)
			{
				for (j=0; j<lfp.n_chans; j++)
					waves[j][IDX(dIdx)*2+1] = waves[j][IDX(dIdx-1)*2+1];
				dIdx++;
			}	
			curSeqNum++;
			//nBuffLost++;
		}		
	}

	
	curSeqNum = lfp.seq_num;
	xPos = waves[0][IDX(dIdx)*2];
}


void idleFn(void){
	do{
		updateWaveArray();
	}while(tryToGetLfp(&lfp));
	eraseWaveformViewport();
	redrawWindow();
//	nBuffLost = 0;
    usleep(IDLE_SLEEP_USEC);
}

void redrawWindow(void)
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
	
	int dxBack = ((waves[0][2] - waves[0][0])*sampleRate/20)/2;
	int dxFront= ((waves[0][2] - waves[0][0])*sampleRate/20)/2;

	glColor3f(0.0, 0.0, 0.0);

	glRecti(x-dxFront/2 , PIX_TO_X(0), x+dxBack/2, yRange);
 
	// EDGE CASES
	// If erase box is clipped on the left, draw it again but shifted all the way to the right and visa versa
	if( (x-dxBack/2) < 0 )
		glRecti(x - dxFront/2 + xRange, PIX_TO_X(0), x + dxBack/2 + xRange, yRange);
	else if( (x+dxFront/2) > xRange)
		glRecti(x - dxFront/2 - xRange, PIX_TO_X(0), x + dxBack/2 - xRange, yRange);



}
void eraseWaveformViewport(){

//	std::cout<<"Clearing all waveforms"<<std::endl;
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
	

//	if (disableWaveOverlay)
//		eraseOldWaveform();
		
	glLoadIdentity();
}


void drawWaveformN(int n){

	if (dIdx==0) // if there isn't any data don't draw it!!!!
		return;

	setWaveformColor(colWave[n]);

	glVertexPointer(2, GL_INT, 0, waves[n]);
	glDrawArrays(GL_LINE_STRIP, 0, IDX(dIdx-2));

 	// only draw the old data if we have looped beyond the edge once
	if (dIdx>maxIdx)
		glDrawArrays(GL_LINE_STRIP, IDX(dIdx+2), maxIdx-IDX(dIdx+2));

}

void setViewportForWaveInfoN(int n){
  float viewDx = xBox;
  float viewDy = yBox+.5;
  float viewX = xPadding-1;
  float viewY = (nChans - (n+1)) * yBox;

  glViewport(viewX, viewY, viewDx, viewDy);

}
void setViewportForWaves(){

	glViewport( xBox+xPadding-1, 0, winWidth-xBox-xPadding, winHeight-yPadding+2 );	// View port uses whole window
	glLoadIdentity ();
	glTranslatef(-1.0, -1.0, 0.0);
	glScalef(xScale, yScale, 0);

}

void setViewportForCommandString(){

    float viewX = 0 + xPadding-1;
    float viewY = 0 + yPadding-2;
    float viewDX = xBox*4;
    float viewDY =  commandWinHeight;

	glViewport(viewX, viewY, viewDX, viewDY);
}

void setWaveformColor(int c){

	int nColor = 14;
	if (c<0)
		c = c * -1;
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
		case 12:
			glColor3f(1.0, 1.0, 1.0);
			break;
		case 13:
			glColor3f(0.0, 0.0, 0.0);
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
		drawString(-.75, -.2, chName[i]);
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
	redrawWindow();
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

void initCommandListAndMap(){
	// List of commands and functions that do not require additional arguments
	quickCmdMap[CMD_CLEAR_WIN] 	= clearWindow;
	quickCmdMap[CMD_SCALE_UP] 	= scaleUp;
	quickCmdMap[CMD_SCALE_DOWN]	= scaleDown;
	quickCmdMap[CMD_SHIFT_UP]	= shiftUp;
	quickCmdMap[CMD_SHIFT_DOWN] = shiftDown;
	quickCmdMap[CMD_RESET_SCALE]= resetScale;
	quickCmdMap[CMD_PREV_COL]	= prevColor;
	quickCmdMap[CMD_NEXT_COL]	= nextColor;
	quickCmdMap[CMD_HELP]		= showHelp;

	quickCmdMap[CMD_RESET_SEQ] 	= resetSeqNum;
	quickCmdMap[CMD_QUIT] 		= quit;

	// List of commands and functions that _DO_ require additional arguments
	slowCmdMap[CMD_GAIN_ALL]	= setGainAll;
	slowCmdMap[CMD_GAIN_SINGLE] = setGainSingle;
	slowCmdMap[CMD_SET_WIN_LEN]	= setWindowTimeLen;
	slowCmdMap[CMD_SET_FRAMERATE]=setFrameRate;
	//	slowCmdMap[CMD_SET_PORT]	= setPortNumber;
	slowCmdMap[CMD_SET_WIN_POSX]= setWindowXPos;
	slowCmdMap[CMD_SET_WIN_POSY]= setWindowYPos;
	slowCmdMap[CMD_SET_WIN_W]	= setWindowWidth;
	slowCmdMap[CMD_SET_WIN_H]	= setWindowHeight;

	slowCmdMap[CMD_SET_LBL_CH00] =setChannelLabel;
	slowCmdMap[CMD_SET_LBL_CH01] =setChannelLabel;
	slowCmdMap[CMD_SET_LBL_CH02] =setChannelLabel;
	slowCmdMap[CMD_SET_LBL_CH03] =setChannelLabel;
	slowCmdMap[CMD_SET_LBL_CH04] =setChannelLabel;
	slowCmdMap[CMD_SET_LBL_CH05] =setChannelLabel;
	slowCmdMap[CMD_SET_LBL_CH06] =setChannelLabel;
	slowCmdMap[CMD_SET_LBL_CH07] =setChannelLabel;


	// List of the above commands that the user is allowed to invoke while the 
	// program is running, the other commands are hidden from the user but 
	// used on start up.
	slowCmdSet.insert(CMD_GAIN_ALL);
	slowCmdSet.insert(CMD_GAIN_SINGLE);
	slowCmdSet.insert(CMD_SET_WIN_LEN);
	slowCmdSet.insert(CMD_SET_FRAMERATE);


	
}
void keyPressedFn(unsigned char key, int x, int y){

//	std::cout<<"Key Pressed value:"<<(int)key<<" key:"<<key<<std::endl;

	switch(CMD_CUR_STATE) 
	{
		case CMD_STATE_QUICK:
			if (quickCmdMap.end() != quickCmdMap.find(key))
			{
//				std::cout<<"Executing Quick Command:"<<key<<std::endl;
				quickCmdMap.find(key)->second();
			}		

			else if(key == KEY_ENTER)
				CMD_CUR_STATE = CMD_STATE_SLOW;
		break;
		
		case CMD_STATE_SLOW:
			if(key==KEY_DEL || key==KEY_ENTER || key==KEY_BKSP)
				CMD_CUR_STATE = CMD_STATE_QUICK;		

			else if (slowCmdMap.end() !=  slowCmdMap.find(key))
			{
				currentCommand = key;
				CMD_CUR_STATE = CMD_STATE_SLOW_ARG;
			}
		break;
	
		case CMD_STATE_SLOW_ARG:
			enterCommandArg(key);
		break;
	}
}


void enterCommandArg(char key){
	switch(key){
		case KEY_BKSP: 
		case KEY_DEL:
			if (cmdStrIdx<=0) 
			{
				CMD_CUR_STATE = CMD_STATE_SLOW;
				return;
			}

			cmd[--cmdStrIdx] = 0; 
			eraseCommandString();
			break;
		case KEY_ENTER: 
			if (cmdStrIdx>0)
			{
				executeCommand((char*)cmd);
				bzero(cmd,cmdStrLen);
				cmdStrIdx = 0;
//				redrawWindow(); 
				currentCommand  = CMD_NULL;
			}
			CMD_CUR_STATE = CMD_STATE_QUICK;
		break;
		default:
			if(key<' ') // ' ' is the first alpha numeric key, we only want those
				return;
			cmd[cmdStrIdx] = key;
			if (cmdStrIdx<CMD_STR_MAX_LEN)
				cmdStrIdx+=1;
	}	
}

bool executeCommand(char *cmdStr){

 	if (slowCmdMap.end() != slowCmdMap.find(currentCommand))
   	{
    	slowCmdMap.find(currentCommand)->second((void*)cmdStr);
		return true;
    }
	return false;
	
}

bool updateWinLen(double l){

	if (l<=0)
	{
		std::cout<<"Invalid window length specified. Please pick a positive number"<<std::endl;
		return false;

	}
	else if ( l==winDt)
	{
		std::cout<<"The specified window length is already in use"<<std::endl;
		return true;
	}

	if (sampleRate * l > MAX_N_SAMP)
	{
		l = MAX_N_SAMP / sampleRate;

		std::cout<<"Invalid window length specified! Given the sampling rate of:"<<sampleRate;
		std::cout<<" the longest valid window is:"<<l<<". Setting window len to:"<<l;
		std::cout<<". If you need a longer window length reduce the sampling rate!"<<std::endl;
	}

	winDt = l;
//	initializeWaveVariables();
	clearWindow();
	return true;
}

void updateFrameRate(int r){
	int minRate = (1000 / MAX_LFP_NET_BUFF_SIZE)+1;
	if(r<minRate)
	{
		std::cout<<"Minimum framerate is "<<minRate<<"Hz, using "<<minRate<<"Hz"<<std::endl;
		r = minRate;
	}
	else if(r>150)
	{
		std::cout<<"Maximum framerate is 150Hz, using 150Hz"<<std::endl;
		r = 150;
	}
	IDLE_SLEEP_USEC = (1e6)/r;
		

}
void setChannelLabel(int n, char* s, int l)
{
	bzero(chName[n], CHAN_NAME_LEN);
	(l >= CHAN_NAME_LEN) ?  memcpy(chName[n], s, CHAN_NAME_LEN) : memcpy(chName[n], s, l);
}

void clearWindow(){
	initializeWaveVariables();
	eraseWaveformViewport();
	dIdx = 0;
	xPos = 0;
}

void resetSeqNum()
{
	curSeqNum = 0;
	clearWindow();
	std::cout<<"Reseting sequence number to 0!"<<std::endl;
}
void quit()
{
	exit(1);
}
void scaleUp()
{
	userScale+=dUserScale;
}
void scaleDown(){
	userScale-=dUserScale;
	if(userScale<1)
		userScale = 1;
}
void shiftUp(){
	userShift+=dUserShift;
}
void shiftDown(){
	userShift-=dUserShift;
}
void resetScale(){
	userScale = 1;
	userShift = 0;
}
void nextColor(){
	colWave[selectedWaveform]++;
	
}
void prevColor(){
	colWave[selectedWaveform]--;

}

void setGainAll(void* v)
{
	std::cout<<"\tSet Gain All -- NOT IMPLEMENTED"<<std::endl;
}
void setGainSingle(void *v)
{
	std::cout<<"\tSet Gain Single -- NOT IMPLEMENTED"<<std::endl;
}
void setWindowTimeLen(void* v)
{	
	double d = atof((char*)v);
	updateWinLen(d);
	
}
void setFrameRate(void *v)
{
	int r = atoi((char*)v);
	updateFrameRate(r);
}
//void setPortNumber(void* v)
//{
//	port = (char*)v;
//}
void setWindowXPos(void* v)
{
	int x = atoi((char*)v);
	winPosX = x;
}
void setWindowYPos(void* v)
{
	int x = atoi((char*)v);
	winPosY = x;
}
void setWindowHeight(void* v)
{
	int x = atoi((char*)v);
	winHeight = x;
}
void setWindowWidth(void* v)
{
	int x = atoi((char*)v);
	winWidth = x;
}
void setChannelLabel(void* v)
{
	int x = currentCommand - 48;
	if(x<0 || x>MAX_N_CHAN)
		return;
	setChannelLabel(x, (char *) v, strlen((char *)v) );
}
void showHelp()
{
	std::cout<<"\n";
	std::cout<<"Simple Commands:\n";
	std::cout<<"\t"<<(char)CMD_CLEAR_WIN	<< " : Clear the window\n";
	std::cout<<"\t"<<(char)CMD_SCALE_UP		<< " : Increase Display Gain\n";
	std::cout<<"\t"<<(char)CMD_SCALE_DOWN	<< " : Decrease Display Gain\n";
	std::cout<<"\t"<<(char)CMD_SHIFT_UP		<< " : Shift waveforms up\n";
	std::cout<<"\t"<<(char)CMD_SHIFT_DOWN	<< " : Shift waveforms down\n";
	std::cout<<"\t"<<(char)CMD_RESET_SCALE	<< " : Reset scaling and shift\n";
	std::cout<<"\t"<<(char)CMD_PREV_COL		<< " : Previous waveform Color\n";
	std::cout<<"\t"<<(char)CMD_NEXT_COL		<< " : Next the window\n";
	std::cout<<" CTRL + Q : Quit\n";
	std::cout<<" CTRL + R : Reset\n";

	std::cout<<"\n";
	std::cout<<"Advanced Commands:";
	std::cout<<" (Press Enter to initiate advanced commands)\n";
	std::cout<<"\t"<<(char)CMD_GAIN_ALL		<<" : (int)\tSet global gains\n";
	std::cout<<"\t"<<(char)CMD_GAIN_SINGLE	<<" : (int)\tSet gains for selected channel\n";
	std::cout<<"\t"<<(char)CMD_SET_WIN_LEN	<<" : (double)\tSet length (in seconds) of the window\n";
	std::cout<<"\t"<<(char)CMD_SET_FRAMERATE<<" : (int)\tSet redraw framerate\n";
	std::cout<<"\t"<<			"#"			<<" : (text)\tSet channel # label \n";
	
}
int incrementIdx(int i){
	if(i==MAX_LFP_NET_BUFF_SIZE-1)
		return 0;
	else
		return i+1;
}

void parseCommandLineArgs(int argc, char**argv)
{
	std::cout<<"Parsing command line args"<<std::endl;
	int c = 0;
	int opt = 0;
	int optionIndex;
	int x;
	while( (c = getopt_long(argc, argv, "", long_options, &optionIndex) )!=-1 )
	{
		currentCommand = c;
		executeCommand(optarg);
	}

}

void dispCommandString()
{
	if (CMD_CUR_STATE == CMD_STATE_SLOW || CMD_CUR_STATE==CMD_STATE_SLOW_ARG)
	{
		setViewportForCommandString();
		//Draw a black rectangle to cover up whatever was previously visible
		glColor3f(0.0, 0.0, 0.0);
		glRectf(-1, -1, 2, 2);
		//Draw the white bounding box
		glColor3f(1.0, 1.0, 1.0);
		drawViewportEdge();

		if(CMD_CUR_STATE == CMD_STATE_SLOW_ARG)	
		{
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
}

void drawString(float x, float y, char *string){

	glRasterPos2f(x, y);

	int len = strlen(string);
	for (int i = 0; i < len; i++) {
    	glutBitmapCharacter(font, string[i]);
	}
}

