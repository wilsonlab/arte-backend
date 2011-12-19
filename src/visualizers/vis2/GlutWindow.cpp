#include "GlutWindow.h"
std::list<ArteTetrodePlot> plots;

void createGlutWindow(int xIn, int yIn, int wIn, int hIn, char *title, int argc, char *argv[]){

	x = xIn;
	y = yIn;
	w = wIn;
	h = hIn;
	nPlots = 1;
	
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize( x, y);
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
		printf("Not ready for GLSL\n");
		exit(1);
	}
	glClearColor(0.0,0.0,0.0,0.0);
	
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
	positionTetrodePlots();
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

	// This is a hackish way to name  each plot, it will go away eventually so don't rely on it
	char temp[100][100] = {0};
	
	for(int i=0; i<nPlots; i++)
	{
		sprintf(temp[i], "Tetrode:%d", i);
		
		std::cout<<"Setting up:"<<temp[i]<<" using port:"<< ports[i]<<"..."<<std::endl;
		TetrodeSource ts = TetrodeSource(ports[i]);
		ArteTetrodePlot tp = ArteTetrodePlot(1,1,1,1,temp[i]);
		tp.setDataSource(ts);
		tp.setEnabled(false);
		tp.initAxes();
					
		tetrodePlots.push_back(tp);
		
	/*	if ((i+1)%nCol==0 && i != 0){
			xax = 0;
			yax += hax;
		}
		else
			xax += wax;*/
	}
	positionTetrodePlots();
	
	glutMainLoop();
}

// Try to force the plots into a square grid
void calcGridSize(){
	// Assume its square and take the square root, if not square add one
	int n = nPlots;
	int cols;
	int rows; 
	cols = (int)sqrt(n);
	// if the square root of number of plots doesn't produce a integer then go up to the next int
	if (n > cols * cols)
		(cols)++;
		
	// assume that N = r * c, if not then add one, 
	//this will leave some empty space but only when n can't be factored
	rows = n / cols;
	if (n > cols * rows)
		rows++;
	
	nRow = rows;
	nCol = cols;
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
//	glClear(GL_COLOR_BUFFER_BIT);

	if(clearNextDraw)
	{
		clearSingleBuffer();
	}
	std::list<ArteTetrodePlot>::iterator i;
	for (i=tetrodePlots.begin(); i!=tetrodePlots.end(); ++i){
		if (!i->getEnabled())
		{
			TetrodeSource * ts = i->getDataSource();
			ts->enableSource();
			i->setEnabled(true);
			continue;
		}
		if(clearNextDraw){
			glFinish();
			i->clearOnNextDraw(true);
		}
		i->redraw();
		
	}
	
	clearNextDraw = false;
	
	glutSwapBuffers();
	glFlush();
	
}
void idleCallback(){
	displayCallback();
	usleep(sleepTime);
}
void positionTetrodePlots(){
//	calcGridSize();
	nCol = 1;
	nRow = 1;
	double xax = 0, yax = 0, wax = w/nCol, hax = h/nRow;
	int plotCount = 0;
//	printf("GlutWindow~positionTetrodePlots() nPlots:%d axWidth:%d, axHeight:%d\n", nPlots, (int)wax, (int)hax);

	std::list<ArteTetrodePlot>::iterator i;	
	for (i=tetrodePlots.begin(); i!=tetrodePlots.end(); ++i)
	{
		i->setPosition(xax,yax,wax,hax);	
		if ((plotCount+1)%nCol==0 && plotCount != 0){
			xax = 0;
			yax += hax;
		}
		else
			xax += wax;
		plotCount++;
	}
	
}
void resizeCallback(int wNew, int hNew){
	
	w = wNew;
	h = hNew;
//	std::cout<<"resizeCallback() w:"<<wNew<<" h:"<<hNew<<std::endl;
	
	clearNextDraw = true;

	clearPlots();

	positionTetrodePlots();
}

void keyPressedCallback(unsigned char key, int x, int y){
	std::cout<<"GlutWindow.keyPressedCallback()"<<std::endl;
	switch(key){
		case 'q':
		exit(0);
		break;
		case 'c':
		clearPlots();
		break;
	}
	if (key=='q')
		exit(0);
//	pc->keyPressedFn(key);
}

void specialKeyCallback(int key, int x, int y){
	std::cout<<"GlutWindow.specialKeyCallback()"<<std::endl;
	// pc->specialKeyFn(key);
}

void mouseClickCallback(int button, int state, int x, int y){
	std::cout<<"GlutWindow.mouseClickCallback()"<<std::endl;
	// pc->mouseClickFn(button, state, x, y);
}

void clearDoubleBuffer(){
	glFinish();
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT);
	
}
void clearSingleBuffer(){
	glFinish();
	glClear(GL_COLOR_BUFFER_BIT);
}
void clearPlots(){
	clearNextDraw = true;
}