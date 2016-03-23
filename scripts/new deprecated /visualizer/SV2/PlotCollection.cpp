#include "PlotCollection.h"

PlotCollection::PlotCollection(int c, int r, int w, int h, char * ports[]){
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

	ports = ports;
					
	font = GLUT_BITMAP_8_BY_13;
	
//	loadAndCompileShader();
	
	printf("New PlotCollection created with %d cols %d rows, and width:%d and height%d", c,r,w,h);
	
	initCommandSets();
}

PlotCollection::~PlotCollection(){
	
}
bool PlotCollection::loadShaderSource(const std::string& filename, std::string& out){

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
void PlotCollection::loadAndCompileShader(){

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
		std::cout<<"Shader is too long it cannot be greater than"<<MAX_SHADER_LEN<<std::endl;
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

	std::cout<<"DONE!\n"<<std::endl;

}
void PlotCollection::initPlots(){
	
	std::cout<<"PlotCollection.cpp - initPlots()"<<std::endl;
	nPlots = 0;
	
	int dWinX = winWidth/nCol;
	int dWinY = (winHeight-cmdWinHeight)/nRow;
	
 	const char* ports[] = {	"6300", "6301", "6302", "6303", "6304", "6305", "6306", "6307",
						"6308", "6309", "6310", "6311", "6312", "6313", "6314", "6315",
						"7000", "7001", "7002", "7003", "7004", "7005", "7006", "7007",
	 					"7008", "7009", "7010", "7011", "7012", "7013", "7014", "7015",
						"7016", "7017", "7018", "7019", "7020",	 "7021", "7022", "7023",

					};
					
	std::cout<<"PlotCollection::initPlots() WARNING! WARNING! WARNING! Not using the user specified ports!"<<std::endl;
	std::cout<<"PlotCollection::initPlots() WARNING! WARNING! WARNING! Not using the user specified ports!"<<std::endl;
	std::cout<<"PlotCollection::initPlots() WARNING! WARNING! WARNING! Not using the user specified ports!"<<std::endl;
	std::cout<<"PlotCollection::initPlots() WARNING! WARNING! WARNING! Not using the user specified ports!"<<std::endl;
	std::cout<<"Each subplot will be "<<dWinX<<" by "<<dWinY<<std::endl;
	for (int i=0; i<nCol; i++)
		for (int j=0; j<nRow; j++)
		{
			plots[nPlots] = new TetrodePlot(dWinX*i, dWinY*(nRow-j-1)+cmdWinHeight, dWinX, dWinY, (char *)ports[nPlots%40]);
			plots[nPlots]->setTetrodeNumber(nPlots);
			plots[nPlots]->initNetworkRxThread();
//			plots[nPlots]->setShaderProgram(shaderProg);
			nPlots++;
		}
		
	cmdWinWidth = plots[nRow*nCol/2 -1 ]->getMaxX();
	selectedPlot = 0;
	plots[selectedPlot]->setSelected(true);
}

void PlotCollection::drawPlot(){
	
//	std::cout<<"PlotCollection::drawPlot()"<<std::endl;

	for (int i=0; i<nPlots; i++)
	{
		plots[i]->draw();
	}
	
	drawCommandString();
	drawAppTitle();
}

void PlotCollection::setViewportForTitle(){
	glViewport(plots[nCol/2 ]->getMinX(), 2, cmdWinWidth-2, cmdWinHeight-2);
}
void PlotCollection::drawAppTitle(){
	glColor3f(1.0,1.0,1.0);
	setViewportForTitle();
//	float xOffset = -1 + 2 * (1 - (9.0 * strlen(app_name) / cmdWinWidth)) - .025;
//	if(xOffset<-.95)
//		xOffset = -.95;
//	drawString(xOffset, -.7, GLUT_BITMAP_9_BY_15, app_name);

}
void PlotCollection::setViewportForCommandWin(){
	glViewport(0,2,cmdWinWidth,cmdWinHeight-2);	
}
void PlotCollection::drawCommandString(){
	setViewportForCommandWin();
	// Draw the viewport edge
	glColor3f(cmdWinCol[0], cmdWinCol[1], cmdWinCol[2]);
	glRecti(-1,-1,2,2);
	glColor3f(1.0,1.0,1.0);
	drawViewportEdge();
	
	drawString(-.99, -.5, font, (char*) ">Begin Typing To Enter A Command");
	switch(cmdState){
	
	}
}

void PlotCollection::resizePlot(int w, int h){

	winWidth = w;
	winHeight =h;
	
	int dWinX = w/nCol;
	int dWinY = (h - cmdWinHeight)/nRow;// - cmdWinHeight - 22/(nRow-1);
	cmdWinWidth = plots[nCol/2 - 1]->getMaxX()+2;
	
	for (int i=0; i<nPlots; i++)
	{
		int c = i%nCol;
		int r = i/nCol;
		
		plots[i]->movePlot(c*dWinX, (nRow-r-1)*dWinY + cmdWinHeight);
		plots[i]->resizePlot(dWinX, dWinY);
	}
	
}

void PlotCollection::scaleUpAll(){
	for (int i=0; i<nPlots; i++)
		plots[i]->scaleUp();
}
void PlotCollection::scaleUpSel(){
	plots[selectedPlot]->scaleUp();
}
void PlotCollection::scaleDownAll(){
	for (int i=0; i<nPlots; i++)
		plots[i]->scaleDown();
}
void PlotCollection::scaleDownSel(){
	plots[selectedPlot]->scaleDown();
}
void PlotCollection::shiftUpAll(){
	for (int i=0; i<nPlots; i++)
		plots[i]->shiftUp();
}
void PlotCollection::shiftUpSel(){
	plots[selectedPlot]->shiftUp();
}
void PlotCollection::shiftDownAll(){
	for(int i=0; i<nPlots; i++)
		plots[i]->shiftDown();
}
void PlotCollection::shiftDownSel(){
	plots[selectedPlot]->shiftDown();
}
void PlotCollection::toggleOverlayAll(){
	allOverlay = !allOverlay;
	for (int i=0; i<nPlots; i++)
		plots[i]->setWaveformOverlay(allOverlay);
}

void PlotCollection::toggleOverlaySel(){
	plots[selectedPlot]->toggleWaveformOverlay();
}
void PlotCollection::clearAll(){
	for (int i=0; i<nPlots; i++)
		plots[i]->clearPlot();
	glClear(GL_COLOR_BUFFER_BIT);
}
void PlotCollection::clearSel(){
	plots[selectedPlot]->clearPlot();
}
void PlotCollection::showHelp(){
	printf("Showing help message");
}
void PlotCollection::quit(){
	exit(1);
}
void PlotCollection::keyPressedFn(unsigned char key){
	// 
	// switch(cmdState)
	//     {
	//         case CMD_STATE_QUICK:
	// 	std::cout<<"PlotCollection::keyPressedFn()"<<std::endl;
	// 
	//             if (quickCmdSet.find(key) != quickCmdSet.end())
				executeQuickCommand(key);
	// 
	//             else if(key == KEY_ENTER)
                cmdState = CMD_STATE_SLOW;
        // break;
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
    // }
}

void PlotCollection::specialKeyFn(int key){
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

void PlotCollection::mouseClickFn(int button, int state, int x, int y){
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

void PlotCollection::initCommandSets(){
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
void PlotCollection::executeQuickCommand(unsigned char key){
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

