#include "SpikeViewer.h"

// TO DO SpikeViewer
// command input
// dynamic number of plots
// command window

// To do TETRODE_PLOT
// implement select(), deselect()
// implement scaleUp(), scaleDown(), setScale(), getScale()
// shiftUp(), shiftDown(), setShift(), getShift()
// clearProjections()
// toggleOverlay()





static int IDLE_SLEEP_USEC = (1e6)/80;
int winWidth = 1262;
int winHeight = 762;

int nCol = 4;
int nRow = 4;

int main( int argc, char** argv )
{

//	initPlots();
	
	glutInit(&argc,argv);
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowPosition( 5, 20);
	glutInitWindowSize( winWidth, winHeight);
	glutCreateWindow("Arte Network Spike Viewer");

	glutReshapeFunc( resizeWinFunc );
	glutIdleFunc( idleFunc );
	glutDisplayFunc( drawTetrodePlots );

	glutKeyboardFunc(keyPressedFn);
	glutSpecialFunc(specialKeyFn);
	/*
	plots[nPlots++] = TetrodePlot(0,300,400,300,"7000");
	plots[nPlots++] = TetrodePlot(400,300,400,300, "7001");
	plots[nPlots++] = TetrodePlot(0,0,400,300, "7002");
	plots[nPlots++] = TetrodePlot(400,0,400,300, "7003");	*/
		initPlots(nCol,nRow);
	for (int i=0; i<nPlots; i++)
		plots[i]->initNetworkRxThread();
		
	plots[selectedPlot]->setSelected(true);
	
	glClear(GL_COLOR_BUFFER_BIT);
	glutMainLoop( );

	return(0);
}

void initPlots(int nCol, int nRow){
	std::cout<<"SpikeViewer.cpp - initPlots()"<<std::endl;
	nPlots = 0;
	
	int dWinX = winWidth/nCol;
	int dWinY = winHeight/nRow;
	
	for (int i=0; i<nCol; i++)
		for (int j=0; j<nRow; j++)
		{
			plots[nPlots] = new TetrodePlot(dWinX*i, dWinY*(nRow-j-1), dWinX, dWinY, "7000");
			nPlots++;
		}
	

//plot = TetrodePlot(0,300,400,300, "7000");
//	plot = MyObject(0,0,0,0,NULL);
//	plots[nPlots++] = TetrodePlot(400,300,400,300, "7001");
//	plots[nPlots++] = TetrodePlot(0,0,400,300, "7002");
//	plots[nPlots++] = TetrodePlot(400,0,400,300, "7004");

}

void drawTetrodePlots(){

//	std::cout<<"SpikeViewer.cpp - drawTetrodePlots()"<<std::endl;
	for (int i=0; i<nPlots; i++)
	{
		plots[i]->draw();
	}
	
	glutSwapBuffers();
	glFlush();
}
void idleFunc(){
//	std::cout<<"SpikeViewer.cpp - idleFunc()"<<std::endl;
	drawTetrodePlots();
	usleep(IDLE_SLEEP_USEC);   
}

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
void keyPressedFn(unsigned char key, int x, int y){
	switch (key){
		case CMD_QUIT:
		exit(1);
		break;
		
		case CMD_SCALE_UP:
		plots[selectedPlot]->scaleUp();
		break;
		
		case CMD_SCALE_DOWN:
		plots[selectedPlot]->scaleDown();
		break;
		
		case CMD_SHIFT_UP:
		plots[selectedPlot]->shiftUp();
		break;
		
		case CMD_SHIFT_DOWN:
		plots[selectedPlot]->shiftDown();
		break;
	}
	std::cout<<"Normal  Key Pressed:"<<(int)key<<", "<<key<<std::endl;
}
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
	
}
