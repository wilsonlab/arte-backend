#include "SDLViewer.h"

int bpp = 0;
int flags = 0;

int main( int argc, char* argv[] )
{
	nCol = 3;
	nRow = 4;

	winWidth = 800;
	winHeight = 600;
	

	initSdl();
	initCommandListAndMap();
	initPlots(nCol,nRow);
		
	plots[selectedPlot]->setSelected(true);

	
	clearWindow();
    while( 1 ) {
        process_events( );
        draw_screen( );
		usleep(SLEEP);
    }

    return 0;
}

static void initSdl(){
// SDL Setup Vars
    const SDL_VideoInfo* info = NULL;  
    


	//Initialiaze SDL Video subsystem, quit if it fails
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        fprintf( stderr, "Video initialization failed: %s\n", SDL_GetError( ) );
        quit(1);
    }
   	//Grab the video informaion, fail if none
    info = SDL_GetVideoInfo( );
    if( !info ) {
        /* This should probably never happen. */
        fprintf( stderr, "Video query failed: %s\n", SDL_GetError() );
        quit(1);
    }

    bpp = info->vfmt->BitsPerPixel;

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    flags = SDL_OPENGL | SDL_RESIZABLE;

    if( SDL_SetVideoMode( winWidth, winHeight, bpp, flags ) == 0 ) {
  	fprintf( stderr, "Video mode set failed: %s\n",
             SDL_GetError( ) );
        quit( 1 );
    }
}
static void quit( int code )
{
    SDL_Quit();
    exit( code );
}

static void process_events( void )
{
    /* Our SDL event placeholder. */
    SDL_Event event;
	
	SDLKey key;
	char *c;
	unsigned char d;
	bool caps;
	
    /* Grab all the events off the queue. */
    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {
		case SDL_MOUSEBUTTONDOWN:
			printf("Mouse Button Clicked\n");
			break;        

		case SDL_KEYDOWN:
		key = event.key.keysym.sym;
		caps = event.key.keysym.mod & (KMOD_SHIFT | KMOD_CAPS);
		keyPressedFn(key, caps);
		specialKeyFn(key);
        break;		

		case SDL_VIDEORESIZE:
		std::cout<<"Resizing!"<<std::endl;
		resizeWindow( event.resize.w, event.resize.h);
		break;
		
        case SDL_QUIT:
            /* Handle quit requests (like Ctrl-c). */
            quit( 0 );
            break;
        }
    }	
}

static void draw_screen( void )
{
    SDL_GL_SwapBuffers( );
	drawTetrodePlots();
}
void initPlots(int nCol, int nRow){
	std::cout<<"SpikeViewer.cpp - initPlots()"<<std::endl;
	nPlots = 0;
	
	int dWinX = winWidth/nCol;
	int dWinY = (winHeight-cmdWinHeight)/nRow;
	
	char* ports[] = {	"6300", "6301", "6302", "6303", "6304", "6305", "6306", "6307",
	 					"7008", "7009", "7010", "7011", "7012", "7013", "7014", "7015",
//	 					"7016", "7017", "7018", "7019", "7020",	 "7021", "7022", "7023",
//	 					"7024", "7025", "7026", "7027", "7028", "7029", "7030", "7031",
					};

	for (int i=0; i<nCol; i++)
		for (int j=0; j<nRow; j++)
		{
			plots[nPlots] = new TetrodePlot(dWinX*i, dWinY*(nRow-j-1)+cmdWinHeight, dWinX, dWinY, ports[nPlots%16]);
			plots[nPlots]->setTetrodeNumber(nPlots);
			plots[nPlots]->initNetworkRxThread();
			nPlots++;
		}
		
	cmdWinWidth = plots[nRow*nCol/2 -1 ]->getMaxX();
		
}

void drawTetrodePlots(){

	for (int i=0; i<nPlots; i++)
	{
		plots[i]->draw();
	}
	
	drawCommandString();
	drawAppTitle();
	// glutSwapBuffers();
	// glFlush();
}

void setViewportForTitle(){
	glViewport(plots[nCol/2 ]->getMinX(), 2, cmdWinWidth-2, cmdWinHeight-2);
}

void drawAppTitle(){
	glColor3f(1.0,1.0,1.0);
	setViewportForTitle();
	float xOffset = -1 + 2 * (1 - (9.0 * strlen(app_name) / cmdWinWidth)) - .025;
	if(xOffset<-.95)
		xOffset = -.95;
//	drawString(xOffset, -.7, GLUT_BITMAP_9_BY_15, app_name);

}
void setViewportForCommandWin(){
	glViewport(0,2,cmdWinWidth,cmdWinHeight-2);	
}

void drawCommandString(){
	setViewportForCommandWin();
	// Draw the viewport edge
	glColor3f(cmdWinCol[0], cmdWinCol[1], cmdWinCol[2]);
	glRecti(-1,-1,2,2);
	glColor3f(1.0,1.0,1.0);
	drawViewportEdge();
	
	//drawString(-.99, -.5, font, ">Begin Typing To Enter A Command");
	switch(cmdState){
	
	}
}

void resizeWindow(int w, int h){	
	winWidth = w;
	winHeight =h;
	
	int dWinX = w/nCol;
	int dWinY = (h - cmdWinHeight)/nRow;// - cmdWinHeight - 22/(nRow-1);
	for (int i=0; i<nPlots; i++)
	{
		int c = i%nCol;
		int r = i/nCol;
		
		plots[i]->movePlot(c*dWinX, (nRow-r-1)*dWinY + cmdWinHeight);
		plots[i]->resizePlot(dWinX, dWinY);
	}
	
	cmdWinWidth = plots[nCol/2 - 1]->getMaxX()+2;

 	if( SDL_SetVideoMode( winWidth, winHeight, bpp, flags ) == 0 ) {
  	fprintf( stderr, "Resize failed: Video mode set failed: %s\n",
             SDL_GetError( ) );
        quit( 1 );
    }
	clearWindow();
//	printf("Resizing window to:%dx%d\n", w,h);
}
static char parseSdlKey(SDLKey k, bool caps){

	char *name = SDL_GetKeyName(k);
	if (strlen(name)>1)
	{
		std::cout<<"Got key:"<<name<<std::endl;
		return 0;
	}
	char c = *name;
	
	if (c>=97 & c<=122 & caps)
		return c - 32;
	else if (c=='[' & caps)
		return '{';
	else if (c==']' & caps)
		return '}';
	else if (c=='-' & caps)
		return '_';
	else if (c=='=' & caps)
		return '+';
	else 
		return c;
}


void keyPressedFn(SDLKey k, bool caps){
	
	char key = parseSdlKey(k,caps);
	
	std::cout<<"KeyPressedFn got:"<<key<<std::endl;
	switch(cmdState)
    {
        case CMD_STATE_QUICK:

            if (quickCmdMap.end() != quickCmdMap.find(key))
            {
              std::cout<<"Executing Quick Command:"<<key<<std::endl;
                quickCmdMap.find(key)->second();
            }

            else if(key == KEY_ENTER)
                cmdState = CMD_STATE_SLOW;
        break;
// 
//         case CMD_STATE_SLOW:
//             if(key==KEY_DEL || key==KEY_ENTER || key==KEY_BKSP)
//                 CMD_CUR_STATE = CMD_STATE_QUICK;
// 
//             else if (slowCmdMap.end() !=  slowCmdMap.find(key))
//             {
//                 currentCommand = key;
//                 CMD_CUR_STATE = CMD_STATE_SLOW_ARG;
//             }
//         break;
// 
//         case CMD_STATE_SLOW_ARG:
// //            enterCommandArg(key);
//         break;

    }
}


void specialKeyFn(SDLKey key){

	plots[selectedPlot]->setSelected(false);
	switch(key){
		case SDLK_LEFT:
			selectedPlot--;
			if (selectedPlot<0 || selectedPlot%nCol == nCol-1)
				selectedPlot+=nCol;
			break;
		case SDLK_RIGHT:
			selectedPlot++;
			if (selectedPlot%nCol==0)
				selectedPlot-=nCol;
			break;
		case SDLK_UP:
			selectedPlot-=nCol;
			if(selectedPlot<0)
				selectedPlot+= nPlots;
			break;
		case SDLK_DOWN:
			selectedPlot+=nCol;
			if(selectedPlot>=nPlots)
				selectedPlot-=nPlots;
			break;
	}

	plots[selectedPlot]->setSelected(true);
}

/*
void mouseClickFn(int button, int state, int x, int y){
	printf("Mouse clicked at:%d,%d\n",x,y);
	for (int i=0; i<nPlots; i++){
		if(plots[i]->containsPoint(x,winHeight-y))
		{
			plots[selectedPlot]->setSelected(false);
			selectedPlot = i;
			plots[selectedPlot]->setSelected(true);
		}
	}
}
*/

void scaleUpAll(){
	for (int i=0; i<nPlots; i++)
		plots[i]->scaleUp();
}
void scaleUpSel(){
	plots[selectedPlot]->scaleUp();
}
void scaleDownAll(){
	for (int i=0; i<nPlots; i++)
		plots[i]->scaleDown();
}
void scaleDownSel(){
	plots[selectedPlot]->scaleDown();
}
void shiftUpAll(){
	for (int i=0; i<nPlots; i++)
		plots[i]->shiftUp();
}
void shiftUpSel(){
	plots[selectedPlot]->shiftUp();
}
void shiftDownAll(){
	for(int i=0; i<nPlots; i++)
		plots[i]->shiftDown();
}
void shiftDownSel(){
	plots[selectedPlot]->shiftDown();
}
void toggleOverlayAll(){
	allOverlay = !allOverlay;
	for (int i=0; i<nPlots; i++)
		plots[i]->setWaveformOverlay(allOverlay);
}

void toggleOverlaySel(){
	plots[selectedPlot]->toggleWaveformOverlay();
}
void clearAll(){
	for (int i=0; i<nPlots; i++)
		plots[i]->clearPlot();
	clearWindow();
}
void clearWindow(){
	 	glClearColor (0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapBuffers( );
		glClear(GL_COLOR_BUFFER_BIT);
}
void clearSel(){
	plots[selectedPlot]->clearPlot();
}
void showHelp(){
	printf("Showing help message");
}
void quit(){
	exit(1);
}

void initCommandListAndMap(){
    // List of commands and functions that do not require additional arguments
	quickCmdMap[CMD_CLEAR_ALL]  = clearAll;
	quickCmdMap[CMD_CLEAR_SEL]  = clearSel;
	quickCmdMap[CMD_SCALE_UP_SEL] 	= scaleUpSel;
	quickCmdMap[CMD_SCALE_UP_ALL] 	= scaleUpAll;
	quickCmdMap[CMD_SCALE_DOWN_SEL] = scaleDownSel; 
	quickCmdMap[CMD_SCALE_DOWN_ALL] = scaleDownAll;
	quickCmdMap[CMD_SHIFT_UP_SEL] 	= shiftUpSel; 
	quickCmdMap[CMD_SHIFT_UP_ALL] 	= shiftUpAll;
	quickCmdMap[CMD_SHIFT_DOWN_SEL] = shiftDownSel;
	quickCmdMap[CMD_SHIFT_DOWN_ALL] = shiftDownAll;
	quickCmdMap[CMD_OVERLAY_SEL] 	= toggleOverlaySel;
	quickCmdMap[CMD_OVERLAY_ALL] 	= toggleOverlayAll;

    quickCmdMap[CMD_HELP]       = showHelp;
//    quickCmdMap[CMD_RESET_SEQ]  = resetSeqNum;
    quickCmdMap[CMD_QUIT]       = quit;

    // List of commands and functions that _DO_ require additional arguments
/*
    slowCmdMap[CMD_GAIN_ALL]    = setGainAll;
    slowCmdMap[CMD_GAIN_SINGLE] = setGainSingle;
    slowCmdMap[CMD_THOLD_ALL]   = setTholdAll;
    slowCmdMap[CMD_THOLD_SINGLE]= setTholdSingle;
    slowCmdMap[CMD_SET_FRAMERATE]=setFrameRate;
    slowCmdMap[CMD_SET_PORT]    = setPortNumber;
    slowCmdMap[CMD_SET_WIN_POSX]= setWindowXPos;
    slowCmdMap[CMD_SET_WIN_POSY]= setWindowYPos;
    slowCmdMap[CMD_SET_WIN_W]   = setWindowWidth;
    slowCmdMap[CMD_SET_WIN_H]   = setWindowHeight;
	slowCmdMap[CMD_SET_WIN_NAME]= setWindowName;
	slowCmdMap[CMD_SET_POST_SAMPS] = setNPostSamps;*/

    // List of the above commands that the user is allowed to invoke while the 
    // program is running, the other commands are hidden from the user but 
    // used on start up.
/*
    slowCmdSet.insert(CMD_GAIN_ALL);
    slowCmdSet.insert(CMD_GAIN_SINGLE);
    slowCmdSet.insert(CMD_SET_WIN_LEN);
    slowCmdSet.insert(CMD_SET_FRAMERATE);
*/
}

