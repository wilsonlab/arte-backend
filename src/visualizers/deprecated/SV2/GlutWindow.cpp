#include "GlutWindow.h"

PlotCollection *pc;

void createGlutWindow(int xIn, int yIn, int wIn, int hIn, char *title, int argc, char *argv[]){

	x = xIn;
	y = yIn;
	w = wIn;
	h = hIn;
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition( x, y);
	glutInitWindowSize( w, h);
	glutCreateWindow(title);
	
	glutReshapeFunc(resizeCallback);
	glutKeyboardFunc(keyPressedCallback);
	glutMouseFunc(mouseClickCallback);
	glutSpecialFunc(specialKeyCallback);
	
	glutIdleFunc(idleCallback);
	glutDisplayFunc(displayCallback);
	
	glewInit();
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
	else {
		printf("Not totally ready   \n");
		exit(1);
	}
	
}
void positionWindow(int xNew, int yNew){
	x = xNew;
	y = yNew;
	glutPositionWindow(x,y);
	windowPositioned = true;
}
void resizeWindow(int wNew, int hNew){
	std::cout<<"Resizing the window"<<std::endl;

	glutReshapeWindow(wNew,hNew);
	resizeCallback(wNew,hNew);
	windowSized = true;
}
void showWindow(){
	/*
	if (!windowPositioned || !windowSized){
		std::cout<<"Error: Window is not positioned or sized!"<<std::endl;
		exit(1);
	}
	*/
	
	if (!portsSpecified){
		std::cout<<"No ports specified!"<<std::endl;
		exit(1);
	}
	if (!layoutDimsSpecified){
		std::cout<<"Layout dimensions have not been specified"<<std::endl;
		exit(1);
	}
	pc = new PlotCollection(nCol, nRow, w, h, ports);

	pc->initPlots();
	
	glutMainLoop();
}

void setPorts(char* portsNew[]){
	ports = portsNew;
	portsSpecified = true;
}
void setLayoutDims(int c, int r){
	nRow = r;
	nCol = c;
	layoutDimsSpecified = true;
}
void displayCallback(){
	glClear(GL_COLOR_BUFFER_BIT);
	pc->drawPlot();
	
	glutSwapBuffers();
	glFlush();
	
}
void idleCallback(){
	displayCallback();
	usleep(sleepTime);
}

void resizeCallback(int wNew, int hNew){
	
	w = wNew;
	h = hNew;
//	std::cout<<"resizeCallback() w:"<<wNew<<" h:"<<hNew<<std::endl;
//	glLoadIdentity();

	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	

	setViewportWithRange(0,0,w,h,0,0,w,h);

	pc->resizePlot(w,h);
	
// 	glViewport(0,0,w,h);
// 	glLoadIdentity();
// 	glTranslatef(-1.0,-1.0, 0.0);
// //	float scaleX = 2.0/float(w);
// //	float scaleY = 2.0/float(h);
// 	std::cout<<"Scaling viewport by:"<<scaleX<<" and "<<scaleY<<std::endl;
// //	glScalef(.05,.05,1.0);
// 	glScalef(scaleX, scaleY, 1.0);

}

void keyPressedCallback(unsigned char key, int x, int y){
	std::cout<<"GlutWindow.keyPressedCallback()"<<std::endl;
	pc->keyPressedFn(key);
}

void specialKeyCallback(int key, int x, int y){
	std::cout<<"GlutWindow.specialKeyCallback()"<<std::endl;
	pc->specialKeyFn(key);
}

void mouseClickCallback(int button, int state, int x, int y){
	std::cout<<"GlutWindow.mouseClickCallback()"<<std::endl;
	pc->mouseClickFn(button, state, x, y);
}