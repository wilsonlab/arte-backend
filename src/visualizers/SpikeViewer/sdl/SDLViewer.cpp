/*
 * SDL OpenGL Tutorial.
 * (c) Michael Vance, 2000
 * briareos@lokigames.com
 *
 * Distributed under terms of the LGPL. 
 */
#include "SDLViewer.h"

//static GLboolean should_rotate = GL_TRUE;

int main( int argc, char* argv[] )
{
    /* Information about the current video settings. */
    const SDL_VideoInfo* info = NULL;
    /* Dimensions of our window. */
    int width = 0;
    int height =0;
    /* Color depth in bits of our window. */
    int bpp = 0;
    /* Flags we will pass into SDL_SetVideoMode. */
    int flags = 0;

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

    flags = SDL_OPENGL;

    if( SDL_SetVideoMode( winWidth, winHeight, bpp, flags ) == 0 ) {
  	fprintf( stderr, "Video mode set failed: %s\n",
             SDL_GetError( ) );
        quit( 1 );
    }

//    setup_opengl( winWidth, winHeight );

	initPlots(nCol,nRow);
		
	plots[selectedPlot]->setSelected(true);

    while( 1 ) {
        process_events( );
        draw_screen( );
    }

    return 0;
}

static void quit( int code )
{
    SDL_Quit();
    exit( code );
}

static void handle_key_down( SDL_keysym* keysym )
{
    switch( keysym->sym ) {
    case SDLK_ESCAPE:
        quit( 0 );
        break;
    default:
        break;
    }
}

static void process_events( void )
{
    /* Our SDL event placeholder. */
    SDL_Event event;

    /* Grab all the events off the queue. */
    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {
        case SDL_KEYDOWN:
            /* Handle key presses. */
            handle_key_down( &event.key.keysym );
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
	int dWinY = winHeight/nRow;
	
	char* ports[] = {	"6300", "6301", "6302", "6303", "6304", "6305", "6306", "6307",
//	 					"7008", "7009", "7010", "7011", "7012", "7013", "7014", "7015",
//	 					"7016", "7017", "7018", "7019", "7020",	 "7021", "7022", "7023",
//	 					"7024", "7025", "7026", "7027", "7028", "7029", "7030", "7031",
					};
	for (int i=0; i<nCol; i++)
		for (int j=0; j<nRow; j++)
		{
			plots[nPlots] = new TetrodePlot(dWinX*i, dWinY*(nRow-j-1), dWinX, dWinY, ports[nPlots%32]);
			plots[nPlots]->setTetrodeNumber(nPlots);
			plots[nPlots]->initNetworkRxThread();
			nPlots++;
		}
}

void drawTetrodePlots(){
	for (int i=0; i<nPlots; i++)
	{
		plots[i]->draw();
	}
}

void idleFunc(){
//	std::cout<<"SpikeViewer.cpp - idleFunc()"<<std::endl;
	drawTetrodePlots();
	usleep(IDLE_SLEEP_USEC);   
}
/*
void resizeWinFunc(int w, int h){
	glClear(GL_COLOR_BUFFER_BIT);
	
	winWidth = w;
	winHeight = h;
	
	int dWinX = w/nCol;
	int dWinY = h/nRow;
	for (int i=0; i<nPlots; i++)
	{
		int c = i%nCol;
		int r = i/nCol;
		
		plots[i]->movePlot(c*dWinX, (nRow-r-1)*dWinY);
		plots[i]->resizePlot(w/nCol, h/nRow);
	}
	printf("Resizing window to:%dx%d\n", w,h);
}
/*
/*
void keyPressedFn(unsigned char key, int x, int y){
	switch (key){
		case CMD_QUIT:
		exit(1);
		break;
		
		case CMD_SCALE_UP_SEL:
		plots[selectedPlot]->scaleUp();
		break;

		case CMD_SCALE_UP_ALL:
		for (int i=0; i<nPlots; i++)
			plots[i]->scaleUp();
		break;

		case CMD_SCALE_DOWN_ALL:
		for (int i=0; i<nPlots; i++)
			plots[i]->scaleDown();
		break;

		case CMD_SCALE_DOWN_SEL:
		plots[selectedPlot]->scaleDown();
		break;
	
		case CMD_SHIFT_UP_SEL:
		plots[selectedPlot]->shiftUp();
		break;
	
		case CMD_SHIFT_UP_ALL:
		for (int i=0; i<nPlots; i++)
			plots[i]->shiftUp();
		break;
		
		case CMD_SHIFT_DOWN_SEL:
		plots[selectedPlot]->shiftDown();
		break;
		
		case CMD_SHIFT_DOWN_ALL:
		for (int i=0; i<nPlots; i++)
			plots[i]->shiftDown();
		break;
		
		case CMD_CLEAR_ALL:
		for (int i=0; i<nPlots; i++)
			plots[i]->clearPlot();
		break;
		
		case CMD_CLEAR_SEL:
		plots[selectedPlot]->clearPlot();
		break;
		
		case CMD_OVERLAY_ALL:
		allOverlay = !allOverlay;
		for (int i=0; i<nPlots; i++)
			plots[i]->setWaveformOverlay(allOverlay);
		break;
		
		case CMD_OVERLAY_SEL:
		plots[selectedPlot]->toggleWaveformOverlay();
		break;
	}
	std::cout<<"Normal  Key Pressed:"<<(int)key<<", "<<key<<std::endl;
}
*/
/*
void specialKeyFn(int key, int x, int y){
	std::cout<<"Special Key Pressed:"<<key<<", "<<(char)key<<std::endl;
	plots[selectedPlot]->setSelected(false);
	switch(key){
		case GLUT_KEY_LEFT:
			selectedPlot--;
			if (selectedPlot<0 || selectedPlot%nCol == nCol-1)
				selectedPlot+=nCol;
			break;
		case GLUT_KEY_RIGHT:
			selectedPlot++;
			if (selectedPlot%nCol==0)
				selectedPlot-=nCol;
			break;
		case GLUT_KEY_UP:
			selectedPlot-=nCol;
			if(selectedPlot<0)
				selectedPlot+= nPlots;
			break;
		case GLUT_KEY_DOWN:
			selectedPlot+=nCol;
			if(selectedPlot>=nPlots)
				selectedPlot-=nPlots;
			break;
	}

	plots[selectedPlot]->setSelected(true);
}*/
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
}*/

