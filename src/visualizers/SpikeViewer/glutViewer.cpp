
#include "glutViewer.h"
static int IDLE_SLEEP_USEC = (1e6)/80;
int winWidth = 1262;
int winHeight = 762;

int nCol = 3;
int nRow = 2;

void *font = GLUT_BITMAP_8_BY_13;
SpikeViewer sv = SpikeViewer(nCol, nRow, winWidth, winHeight, NULL);

int main( int argc, char** argv )
{

	glutInit(&argc,argv);
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowPosition( 5, 20);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Arte Network Spike Viewer");

	glutReshapeFunc( resizeWinFunc );
	glutIdleFunc( idleFunc );
	glutDisplayFunc( redraw );

	glutKeyboardFunc(keyPressedFn);
	glutSpecialFunc(specialKeyFn);
	glutMouseFunc(mouseClickFn);

	glewInit();
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
	else {
		printf("Not totally ready   \n");
		exit(1);
	}
	
	sv.initPlots();
	sv.loadAndCompileShader();
	
			
	glClear(GL_COLOR_BUFFER_BIT);
	glutMainLoop( );

	return(0);
}

void redraw(){

	sv.drawPlot();
	glutSwapBuffers();
	glFlush();
}
void idleFunc(){
//	std::cout<<"SpikeViewer.cpp - idleFunc()"<<std::endl;
	redraw();
	usleep(IDLE_SLEEP_USEC);   
}

void clearWindow(){
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT);
}

void resizeWinFunc(int w, int h){
	sv.resizePlot(w,h);
	clearWindow();
}

void keyPressedFn(unsigned char key, int x, int y){
	sv.keyPressedFn(key);
}

void specialKeyFn(int key, int x, int y){
	sv.specialKeyFn(key);
}

void mouseClickFn(int button, int state, int x, int y){
	sv.mouseClickFn(button, state, x, y);
}


