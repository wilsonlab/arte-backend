#include <GL/glew.h>

#include "SpikeViewer.h"

#include <GL/glut.h>



SpikeViewer::SpikeViewer(int c, int r, int w, int h, char * ports[]){
	nRow = r;
	nCol = c;
	winWidth = w;
	winHeight = h;
	
	nPlots = 0;
	selectedPlot = 0;
	allOverlay = true;
	
	cmdStrIdx = 0;
	cmdWinHeight=22;
	cmdWinWidth = 100;

	cmdWinCol[0] = .2;
	cmdWinCol[1] = .2;
	cmdWinCol[2] = .2;
	cmdState = CMD_STATE_QUICK;
	
	currentCommand = 0;
	enteringCommand = false;

	font = GLUT_BITMAP_8_BY_13;
	
	//	loadAndCompileShader(); //was commented out
	
	initCommandSets();
}

SpikeViewer::~SpikeViewer(){
	
}
bool SpikeViewer::loadShaderSource(const std::string& filename, std::string& out){

	std::ifstream file;
    file.open(filename.c_str());
    if (!file) {
	       return false;
    }

	std::stringstream stream;

	stream << file.rdbuf();
	file.close();

	out = stream.str();

	return true;

}
void SpikeViewer::loadAndCompileShader(){

	GLuint axesShader = glCreateShader(GL_VERTEX_SHADER);

	const std::string shaderfile = "cluster.vert";
	std::string shadersource;
	std::cout<<"Reading shader source from file:"<<shaderfile<<std::endl;
	
	if ( !loadShaderSource(shaderfile, shadersource) )
	{
		std::cout<<"Failed to load shader:"<<shaderfile<<" quiting!"<<std::endl;
		exit(1);
	}

	const int MAX_SHADER_LEN = 10000;
	if (shadersource.size()>MAX_SHADER_LEN)
	{
		std::cout<<"Shader is too long it cannot be greater than" <<MAX_SHADER_LEN<<std::endl;
		exit(1);
	}

	GLchar *source = new GLchar[MAX_SHADER_LEN];
	for (int i=0; i<MAX_SHADER_LEN; i++)
		source[i] = 0;
		
	memcpy(source, shadersource.c_str(), shadersource.size());
	
 	int len = strlen(source);
	glShaderSource(axesShader, 1, (const GLchar**) &source, (const GLint *) &len);
	glCompileShader(axesShader);
	shaderProg = glCreateProgram();
	glAttachShader(shaderProg, axesShader);
	glLinkProgram(shaderProg);

	std::cout<<"DONE NOW!\n"<<std::endl;

}

void SpikeViewer::initPlots(){
	std::cout<<"SpikeViewer.cpp - initPlots()"<<std::endl;
	nPlots = 0;
	
	int dWinX = winWidth/nCol;
	int dWinY = (winHeight-cmdWinHeight)/nRow;
	
	//enter own ports	
	using namespace std;
		cout << "enter a port number between 1000-9999-- the first port used will be the port after your input: "; // gets port from user input
		cin >> startingport;


	for (int i=0; i<32; i++) // defines 32 ports starting with user input
		{
		  portarray[i] = new char[6];
		  startingport=startingport+1;
		  sprintf(portarray[i], "%d", startingport);
		  // cout << "defining port: " << portarray[i] << endl;
		}

	//next line was commented out
	//	char* ports[] = {};

	for (int i=0; i<nCol; i++)
		for (int j=0; j<nRow; j++) 
		{
		  plots[nPlots] = new TetrodePlot(dWinX*i, dWinY*(nRow-j-1)+cmdWinHeight, dWinX, dWinY, portarray[nPlots%16]); 
		  plots[nPlots]->setTetrodeNumber(nPlots);
		  plots[nPlots]->initNetworkRxThread(); 
		  //already commented out	 plots[nPlots]->setShaderProgram(shaderProg);
			nPlots++;
		}
		
	cmdWinWidth = plots[nRow*nCol/2 -1 ]->getMaxX();
	selectedPlot = 0;
	plots[selectedPlot]->setSelected(true);

}

void SpikeViewer::drawPlot(){

	glClear(GL_COLOR_BUFFER_BIT);

	for (int i=0; i<nPlots; i++)
	{
		plots[i]->draw();
	}
	
	drawCommandString();
	drawAppTitle();
}

void SpikeViewer::setViewportForTitle(){
	glViewport(plots[nCol/2 ]->getMinX(), 2, cmdWinWidth-2, cmdWinHeight-2);
}
void SpikeViewer::drawAppTitle(){
	glColor3f(1.0,1.0,1.0);
	setViewportForTitle();
//	float xOffset = -1 + 2 * (1 - (9.0 * strlen(app_name) / cmdWinWidth)) - .025;
//	if(xOffset<-.95)
//		xOffset = -.95;
//	drawString(xOffset, -.7, GLUT_BITMAP_9_BY_15, app_name);

}
void SpikeViewer::setViewportForCommandWin(){
	glViewport(0,2,cmdWinWidth,cmdWinHeight-2);	
}
void SpikeViewer::drawCommandString(){
	setViewportForCommandWin();
	// Draw the viewport edge
	glColor3f(cmdWinCol[0], cmdWinCol[1], cmdWinCol[2]);
	glRecti(-1,-1,2,2);
	glColor3f(1.0,1.0,1.0);
	drawViewportEdge();
	char begintyping[] = "Being typing to enter a command: ";
	drawString(-.99, -.5, font, begintyping);
	switch(cmdState){
	
	}
}

void SpikeViewer::resizePlot(int w, int h){

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
}

void SpikeViewer::scaleUpAll(){
	for (int i=0; i<nPlots; i++)
		plots[i]->scaleUp();
}
void SpikeViewer::scaleUpSel(){
	plots[selectedPlot]->scaleUp();
}
void SpikeViewer::scaleDownAll(){
	for (int i=0; i<nPlots; i++)
		plots[i]->scaleDown();
}
void SpikeViewer::scaleDownSel(){
	plots[selectedPlot]->scaleDown();
}
void SpikeViewer::shiftUpAll(){
	for (int i=0; i<nPlots; i++)
		plots[i]->shiftUp();
}
void SpikeViewer::shiftUpSel(){
	plots[selectedPlot]->shiftUp();
}
void SpikeViewer::shiftDownAll(){
	for(int i=0; i<nPlots; i++)
		plots[i]->shiftDown();
}
void SpikeViewer::shiftDownSel(){
	plots[selectedPlot]->shiftDown();
}
void SpikeViewer::toggleOverlayAll(){
	allOverlay = !allOverlay;
	for (int i=0; i<nPlots; i++)
		plots[i]->setWaveformOverlay(allOverlay);
}

void SpikeViewer::toggleOverlaySel(){
	plots[selectedPlot]->toggleWaveformOverlay();
}
void SpikeViewer::clearAll(){
	for (int i=0; i<nPlots; i++)
		plots[i]->clearPlot();
	glClear(GL_COLOR_BUFFER_BIT);
}
void SpikeViewer::clearSel(){
	plots[selectedPlot]->clearPlot();
}
void SpikeViewer::showHelp(){
	printf("Showing help message");
}
void SpikeViewer::quit(){
	exit(1);
}
void SpikeViewer::keyPressedFn(unsigned char key){
	switch(cmdState)
    {
        case CMD_STATE_QUICK:
		std::cout<<"SpikeViewer::keyPressedFn()"<<std::endl;

            if (quickCmdSet.find(key) != quickCmdSet.end())
				executeQuickCommand(key);

            else if(key == KEY_ENTER)
                cmdState = CMD_STATE_SLOW;
        break;
/*
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
//            enterCommandArg(key);
        break;
*/
    }
}

void SpikeViewer::specialKeyFn(int key){
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

void SpikeViewer::mouseClickFn(int button, int state, int x, int y){
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

void SpikeViewer::initCommandSets(){
    // List of quick one key commands
	quickCmdSet.insert(CMD_CLEAR_ALL);
	quickCmdSet.insert(CMD_CLEAR_SEL);
	quickCmdSet.insert(CMD_SCALE_UP_SEL);
	quickCmdSet.insert(CMD_SCALE_UP_ALL);
	quickCmdSet.insert(CMD_SCALE_DOWN_SEL);
	quickCmdSet.insert(CMD_SCALE_DOWN_ALL);
	quickCmdSet.insert(CMD_SHIFT_UP_SEL);
	quickCmdSet.insert(CMD_SHIFT_UP_ALL);
	quickCmdSet.insert(CMD_SHIFT_DOWN_SEL);
	quickCmdSet.insert(CMD_SHIFT_DOWN_ALL);
	quickCmdSet.insert(CMD_OVERLAY_SEL);
	quickCmdSet.insert(CMD_OVERLAY_ALL);

	quickCmdSet.insert(CMD_QUIT);
	quickCmdSet.insert(CMD_HELP);
}
void SpikeViewer::executeQuickCommand(unsigned char key){
	std::cout<<"executing quick command:"<<key<<std::endl;
	switch(key){
		case CMD_CLEAR_ALL:
		clearAll();
		break;

		case CMD_CLEAR_SEL:
		clearSel();
		break;

		case CMD_SCALE_UP_SEL:
		scaleUpSel();
		break;

		case CMD_SCALE_UP_ALL:
		scaleUpAll();
		break;

		case CMD_SCALE_DOWN_SEL:
		scaleDownSel();
		break;

		case CMD_SCALE_DOWN_ALL:
		scaleDownAll();
		break;

		case CMD_SHIFT_UP_ALL:
		shiftUpAll();
		break;

		case CMD_SHIFT_UP_SEL:
		shiftUpSel();
		break;

		case CMD_SHIFT_DOWN_ALL:
		shiftDownAll();
		break;
		
		case CMD_SHIFT_DOWN_SEL:
		shiftDownSel();
		break;
		
		case CMD_OVERLAY_SEL:
		toggleOverlaySel();
		break;
		
		case CMD_OVERLAY_ALL:
		toggleOverlayAll();
		break;
		
		case CMD_HELP:
		showHelp();
		break;
		
		case CMD_QUIT:
		quit();
		break;
		
	}
}

