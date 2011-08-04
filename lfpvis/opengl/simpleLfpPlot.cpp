#include "simpleLfpPlot.h"

int main( int argc, char** argv )
{
	
	for (int i=0; i<MAX_N_CHAN; i++)
		setChannelLabel(i, defName, CHAN_NAME_LEN);	
//	memcpy(chName[i], &"HPC N:M", CHAN_NAME_LEN);

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
	net = NetCom::initUdpRx(host,port);
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
	
	int i=0, j=0, k = 0;
	
	// if this is the first time we've run updateWave then seqNum-1 as now
//	std::cout<<"curSeqNum:"<<curSeqNum<<" lfp.seq_num:"<<lfp.seq_num<<std::endl;

	if(curSeqNum==0 && lfp.seq_num>0)
		curSeqNum = lfp.seq_num-1;

//	std::cout<<"curSeqNum:"<<curSeqNum<<" lfp.seq_num:"<<lfp.seq_num<<std::endl;

	
	
	if (lfp.seq_num < curSeqNum)  // If this packet is old ignore it
	{
//		std::cout<<"Old packet received, ignoring it! ";//<<lfp.seq_num<<" curSeqNum"<<curSeqNum<<std::endl;
//		std::cout<<"Press CTRL+R to reset sequence number:"<<std::endl;
		return;
	}
	
	else if(lfp.seq_num == curSeqNum) // if we have the same packet as last time
		return;
	
	else if(lfp.seq_num == curSeqNum+1) // if we have the NEXT packet in the sequence
	{	
		for (i=0; i<lfp.n_samps_per_chan; i++)
		{
			for (j=0; j<lfp.n_chans; j++)
				waves[j][IDX(dIdx)*2+1] = SCALE_VOLTAGE(lfp.data[k++],j);
			dIdx++;
		}
	}
	// If somehow the next packet is 
	else if(lfp.seq_num > curSeqNum+1)
	{
		while(lfp.seq_num > curSeqNum+1) // if the packet is not the next packet (ie packets got dropped)
		{
		
			int validIdx = dIdx;
			//set the next data to 0 and advanced the seqNum
			for (i=0; i<lfp.n_samps_per_chan; i++)
			{
				for (j=0; j<lfp.n_chans; j++)
					//waves[j][IDX(dIdx)*2+1] = SCALE_VOLTAGE(0,j);
					waves[j][IDX(dIdx)*2+1] = waves[j][IDX(dIdx-1)*2+1];
				dIdx++;
			}	
			curSeqNum++;
			nBuffLost++;
//			std::cout<<"NBuff Lost:"<<nBuffLost<<std::endl;
		}		
//		clearWaveforms();
	}

	
	curSeqNum = lfp.seq_num;
	xPos = waves[0][IDX(dIdx)*2];
}


void idleFn(void){
	do{
		updateWaveArray();
	}while(tryToGetLfp(&lfp));
	clearWaveforms();
	redrawWindow();
	nBuffLost = 0;
    usleep(IDLE_SLEEP_USEC);
//	std::cout<<"idleFn Sleeping"<<std::endl;
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
void clearWaveforms(){

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
		case CMD_SET_FRAMERATE:
		case CMD_SET_WIN_LEN:
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
			redrawWindow(); // to erase the command window if no lfps are coming in
			currentCommand  = CMD_NULL;
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

bool executeCommand(char *cmdStr){
	int len = sizeof(cmdStr);
	int x;
	std::cout<<"Executing command:"<<currentCommand<<":"<<cmdStr<<std::endl;

	switch(currentCommand){
		case CMD_SET_WIN_LEN:
			std::cout<<"Changing window len to:"<<cmdStr<<std::endl;
//			atof(cmd);			
			updateWinLen(atof(cmdStr));
		break;

		case CMD_SET_FRAMERATE:
//			int r = atoi(cmd);
			updateFrameRate(atoi(cmdStr));
		break;
		case ARG_PORT_NUM:
			port = cmdStr;
            std::cout<<"Setting port to:"<<port<<std::endl;
        break;
		case ARG_WIN_NAME:
			std::cout<<"setting the window title:"<<optarg<<std::endl;
			bzero(windowTitle, 200);
			memcpy(windowTitle, optarg, strlen(optarg));				
		break;
		case ARG_WIN_POSX:
			x = atoi(cmdStr);
			if (x>=0)
				winPosX = x;					
		break;
		case ARG_WIN_POSY:
		 	x = atoi(cmdStr);
			if (x>=0)
				winPosY = x;					
		break;
		case ARG_WIN_W:
			x = atoi(optarg);
			if (x<=0){
				std::cout<<"Invalid window width chosen, must be greater than 0"<<std::endl;
				exit(-1);
			}
			winWidth = x;
		break;
		case ARG_WIN_H:
			x = atoi(optarg);
			if (x<=0){
				std::cout<<"Invalid window height chosen, must be greater than 0"<<std::endl;
				exit(-1);
			}
			winHeight = x;
		break;
		case CH00:
		case CH01:
		case CH02:
		case CH03:
		case CH04:
		case CH05:
		case CH06:
		case CH07:
			setChannelLabel(currentCommand, cmdStr, strlen(cmdStr));
		break;
	}
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
	initializeWaveVariables();
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
	dIdx = 0;
	xPos = 0;
}

void toggleOverlay(){
	disableWaveOverlay = !disableWaveOverlay;
	if(disableWaveOverlay)
		clearWindow();
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
	while( (c = getopt_long(argc, argv, "n:x:y:w:h:p:r:", long_options, &optionIndex) )!=-1 )
	{
		currentCommand = c;
		executeCommand(optarg);
		/*
		switch(c){
		} */

	}

}
