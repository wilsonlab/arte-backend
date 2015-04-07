
#include "simpleSpikePlot.h"

void *getNetSpike(void *ptr);




#define GLUT_ICON "icon.72.png"
int main( int argc, char** argv ) 
{
	initCommandListAndMap();
//	parseCommandLineArgs(argc, argv);
//	askport();

	if(strlen(windowTitle)==0)
		memcpy(windowTitle, &"Arte Network Spike Viewer", 25);

	bzero(cmd, cmdStrLen);

	void (*prev_fn)(int);
	prev_fn = signal(SIGUSR1, signalUser1);


	pthread_t netThread;
	net = NetCom::initUdpRx(host,port);
	//net = NetCom::initUdpRx(host,port); // here is the mess up
	pthread_create(&netThread, NULL, getNetSpike, (void *)NULL);

	srand(time(NULL));
	gettimeofday(&startTime,NULL);


	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowPosition( winPosX, winPosY);
	glutInitWindowSize( winWidth, winHeight);
	glutCreateWindow(windowTitle);

	glutReshapeFunc( resizeWindow );
	glutIdleFunc( idleFn );
	glutDisplayFunc( refreshDrawing );

	glutKeyboardFunc(keyPressedFn);
	glutSpecialFunc(specialKeyFn);


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
		spike_net_t t;
		t = s;
		spikeBuff[writeIdx] = t;
		writeIdx = incrementIdx(writeIdx);
		nSpikes+=1;
		totalSpikesRead++;
	}
}


void idleFn(void){
	do{
		refreshDrawing();
	}while(tryToGetSpike(&spike));
    usleep(IDLE_SLEEP_USEC);
	
}


void refreshDrawing(void)
{
	if (disableWaveOverlay)
		eraseWaveforms();

	highlightSelectedWaveform();
	drawWaveforms();
	drawProjections();
	drawBoundingBoxes();
	dispCommandString();

	glutSwapBuffers();
	glFlush();
}

void eraseWaveforms(){

	glViewport(xPadding, yPadding, xBox-xPadding, yBox*2 - yPadding);

	glColor3f(0,0,0);
	glRectf(-1, -1, 2, 2);

}

void eraseCommandString(){
	setViewportForCommandString();
	glColor3f(0,0,0);
	glRectf(-1, -1, 2, 2);
}


void highlightSelectedWaveform(){
	setViewportForWaveN(selectedWaveform);
	glColor3f(colSelected[0], colSelected[1], colSelected[2]);
	glRecti(-1, -1, 2, 2);
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
	switch(CMD_CUR_STATE)
    {
        case CMD_STATE_QUICK:
            if (quickCmdMap.end() != quickCmdMap.find(key))
            {
//              std::cout<<"Executing Quick Command:"<<key<<std::endl;
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
//              redrawWindow(); 
                currentCommand  = CMD_NULL;
            }
            CMD_CUR_STATE = CMD_STATE_QUICK;
        break;
        default:
            if(key<' ') // ' ' is the first alpha numeric key, we only want tho$
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

void initCommandListAndMap(){
    // List of commands and functions that do not require additional arguments
    quickCmdMap[CMD_CLEAR_WIN]  = clearWindow;
    quickCmdMap[CMD_SCALE_UP]   = scaleUp;
    quickCmdMap[CMD_SCALE_DOWN] = scaleDown;
    quickCmdMap[CMD_SHIFT_UP]   = shiftUp;
    quickCmdMap[CMD_SHIFT_DOWN] = shiftDown;
    quickCmdMap[CMD_RESET_SCALE]= resetScale;
    quickCmdMap[CMD_HELP]       = showHelp;

    quickCmdMap[CMD_RESET_SEQ]  = resetSeqNum;
    quickCmdMap[CMD_QUIT]       = quit;

    // List of commands and functions that _DO_ require additional arguments
    slowCmdMap[CMD_GAIN_ALL]    = setGainAll;
    slowCmdMap[CMD_GAIN_SINGLE] = setGainSingle;
    slowCmdMap[CMD_THOLD_ALL]   = setTholdAll;
    slowCmdMap[CMD_THOLD_SINGLE]= setTholdSingle;
    slowCmdMap[CMD_SET_FRAMERATE]=setFrameRate;
   // slowCmdMap[CMD_SET_PORT]    = setPortNumber; //??
    slowCmdMap[CMD_SET_WIN_POSX]= setWindowXPos;
    slowCmdMap[CMD_SET_WIN_POSY]= setWindowYPos;
    slowCmdMap[CMD_SET_WIN_W]   = setWindowWidth;
    slowCmdMap[CMD_SET_WIN_H]   = setWindowHeight;
	slowCmdMap[CMD_SET_WIN_NAME]= setWindowName;
	slowCmdMap[CMD_SET_POST_SAMPS] = setNPostSamps;

    // List of the above commands that the user is allowed to invoke while the 
    // program is running, the other commands are hidden from the user but 
    // used on start up.
    slowCmdSet.insert(CMD_GAIN_ALL);
    slowCmdSet.insert(CMD_GAIN_SINGLE);
    slowCmdSet.insert(CMD_SET_WIN_LEN);
    slowCmdSet.insert(CMD_SET_FRAMERATE);

}

void updateFrameRate(int r){
    int minRate = 10;
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
	clearWindow();
}

void setGainAll(void* v)
{
    std::cout<<"\tSet Gain All -- NOT IMPLEMENTED"<<std::endl;
}
void setGainSingle(void *v)
{
    std::cout<<"\tSet Gain Single -- NOT IMPLEMENTED"<<std::endl;
}

void setTholdAll(void* v)
{
    std::cout<<"\tSet Thold All -- NOT IMPLEMENTED"<<std::endl;
}
void setTholdSingle(void *v)
{
    std::cout<<"\tSet Thold Single -- NOT IMPLEMENTED"<<std::endl;
}
void setNPostSamps(void *v)
{
	std::cout<<"\tSet N Post Samps -- NOT IMPLEMENTED"<<std::endl;
}

void setFrameRate(void *v)
{
    int r = atoi((char*)v);
    updateFrameRate(r);
}


//void setPortNumber(void* v)
//{
//    port = (char*)v;
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
void setWindowName(void *v){
	int len = strlen((char*)v);
	memcpy(&windowTitle, v, len);
}


void showHelp()
{
    std::cout<<"\n";
    std::cout<<"Simple Commands:\n";
    std::cout<<"\t"<<(char)CMD_CLEAR_WIN    << " : Clear the window\n";
    std::cout<<"\t"<<(char)CMD_SCALE_UP     << " : Increase Display Gain\n";
    std::cout<<"\t"<<(char)CMD_SCALE_DOWN   << " : Decrease Display Gain\n";
    std::cout<<"\t"<<(char)CMD_SHIFT_UP     << " : Shift waveforms up\n";
    std::cout<<"\t"<<(char)CMD_SHIFT_DOWN   << " : Shift waveforms down\n";
    std::cout<<"\t"<<(char)CMD_RESET_SCALE  << " : Reset scaling and shift\n";
    std::cout<<"\t"<<(char)CMD_PREV_COL     << " : Previous waveform Color\n";
    std::cout<<"\t"<<(char)CMD_NEXT_COL     << " : Next the window\n";
    std::cout<<" CTRL + Q : Quit\n";
    std::cout<<" CTRL + R : Reset\n";

    std::cout<<"\n";
    std::cout<<"Advanced Commands:";
    std::cout<<" (Press Enter to initiate advanced commands)\n";
    std::cout<<"\t"<<(char)CMD_GAIN_ALL     <<" : (int)\tSet global gains\n";
    std::cout<<"\t"<<(char)CMD_GAIN_SINGLE  <<" : (int)\tSet gains for selected channel\n";
    std::cout<<"\t"<<(char)CMD_SET_FRAMERATE<<" : (int)\tSet redraw framerate\n";

}


//void parseCommandLineArgs(int argc, char**argv)
//{
//    std::cout<<"Parsing command line args"<<std::endl;
//    int c = 0;
//    int opt = 0;
//    int optionIndex;
 //   int x;
//    while( (c = getopt_long(argc, argv, "", long_options, &optionIndex) )!=-1 )
//    {
//        currentCommand = c;
//        executeCommand(optarg);
//    }
//
//}


void dispCommandString(){

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
	if(i==MAX_SPIKE_BUFF_SIZE-1)
		return 0;
	else
		return i+1;
}

void signalUser1(int param){
	std::cout<<"Recieved signal:"<<param<<std::endl;
	clearWindow();
}


