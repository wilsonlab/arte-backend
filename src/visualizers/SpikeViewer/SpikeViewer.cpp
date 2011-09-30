#include "SpikeViewer.h"


static int IDLE_SLEEP_USEC = (1e6)/40;

int main( int argc, char** argv )
{

//	initPlots();
	
	glutInit(&argc,argv);
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowPosition( 5, 20);
	glutInitWindowSize( 800, 600);
	glutCreateWindow("Arte Network Spike Viewer");

	glutReshapeFunc( resizeWinFunc );
	glutIdleFunc( idleFunc );
	glutDisplayFunc( drawTetrodePlots );

//	glutKeyboardFunc(keyPressedFn);
//	glutSpecialFunc(specialKeyFn);
	plots[nPlots++] = TetrodePlot(0,300,400,300,"7000");
	plots[nPlots++] = TetrodePlot(400,300,400,300, "7001");
	plots[nPlots++] = TetrodePlot(0,0,400,300, "7002");
	plots[nPlots++] = TetrodePlot(400,0,400,300, "7003");	
	for (int i=0; i<nPlots; i++)
		plots[i].initNetworkRxThread();
		
	glClear(GL_COLOR_BUFFER_BIT);
	glutMainLoop( );

	return(0);
}

void initPlots(){
	std::cout<<"SpikeViewer.cpp - initPlots()"<<std::endl;
	nPlots = 0;

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
		plots[i].draw();
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
	
	for (int i=0; i<nPlots; i++)
		plots[i].resizePlot(w/2, h/2);
	plots[0].movePlot(0/2, h/2);
	plots[1].movePlot(w/2, h/2);
	plots[2].movePlot(0/2, 0/2);
	plots[3].movePlot(w/2, 0/2);
}
