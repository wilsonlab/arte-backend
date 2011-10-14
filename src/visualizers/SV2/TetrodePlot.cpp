
#include "TetrodePlot.h"

TetrodePlot::TetrodePlot(){
}
TetrodePlot::TetrodePlot(int x, int y, int w, int h, char *p){
	std::cout<<"Initializing a new TetrodePlot\n"<<std::flush;
	xPos = x, yPos = y;
	plotWidth = w, plotHeight = h;
	padLeft = 1;
	padRight = 1;
	padBottom = 1;
	padTop = 1;

	resizePlot(plotWidth, plotHeight);

	disableWaveOverlay = true;
	waveformLineWidth = 1;
	memcpy(host, "127.0.0.1", 10);
	host[10] = 0;
	port = p;
	
	bzero(txtDispBuff,40);

	// ===================================
	// 		Scaling Variables
	// ===================================

	
	userScale = .4;
	dUserScale = .3;

	userShift = MAX_VOLT*1/4;
	dUserShift = .05;
	
	minAmpRange = MIN_VOLT*userScale+userShift;
	maxAmpRange = MAX_VOLT*userScale+userShift;

	nChan=4;
	nProj=6;
	
	spike = spike_net_t();
	
	font = GLUT_BITMAP_8_BY_13;
	titleFont = GLUT_BITMAP_8_BY_13;
//	bzero(&spikeBuff, sizeof(spikeBuff));
//	spikeBuff = new spike_net_t[MAX_SPIKE_BUFF_SIZE];
	
	nSpikes = 0;
	readIdx = 0;
	writeIdx = 0;
	curSeqNum = 0;
	totalSpikesRead = 0;
	
	nProjPoints=0;
	newIdx=0;
	
	isSelected = false;
	newSpike = false;
	tetrodeNumber=-1;
	
	initColors();
	initVBO();
	
}

void TetrodePlot::initColors(){
	colSelected[0] = 0.2;
	colSelected[1] = 0.2;
	colSelected[2] = 0.2;
	
	colWave[0] = 1.0;
	colWave[1] = 1.0;
	colWave[2] = 0.6;
	
	colThres[0] = 1.0;
	colThres[1] = 0.1;
	colThres[2] = 0.1;
	
	colFont[0] = 1.0;
	colFont[1] = 1.0;
	colFont[2] = 1.0;
		
	colTitle[0] = 0.15;
	colTitle[1] = 0.15; 
	colTitle[2] = 0.5;
	
	colTitleSelected[0] = 0.15;
	colTitleSelected[1] = 0.5; 
	colTitleSelected[2] = 0.15;
}
void TetrodePlot::initVBO(){
		
		SpikeVertex baseData[MAX_N_PROJ_POINTS];
		GLuint indices[MAX_N_PROJ_POINTS];
		countInVBO = 0;
		
		std::cout<<"Created starting data structures, they are:"<< sizeof(SpikeVertex) * MAX_N_PROJ_POINTS / 1024 <<" Kilobytes"<<std::endl;
		std::cout<<std::flush;

		for (int i=0; i<MAX_N_PROJ_POINTS; i++){
			baseData[i].x = 0;//rand()%MAX_VOLT*2 + MIN_VOLT;
			baseData[i].y = 0;//rand()%MAX_VOLT*2 + MIN_VOLT;
			baseData[i].z = 0;//rand()%MAX_VOLT*2 + MIN_VOLT;
			baseData[i].w = 1;//rand()%MAX_VOLT*2 + MIN_VOLT;
			indices[i] = i;
		}

		glGenBuffers(1, &VertexVBOID);
		glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SpikeVertex)*MAX_N_PROJ_POINTS, baseData, GL_DYNAMIC_DRAW);
		// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*MAX_N_POINT, baseData);

		glGenBuffers(1, &IndexVBOID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*MAX_N_PROJ_POINTS, indices, GL_STATIC_DRAW);
	//	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint)*MAX_N_POINT, indices);
//		glBindBuffer(1,0);
		checkGlError();
	
}
void TetrodePlot::resizePlot(int w, int h)
{
	plotWidth = w - padLeft - padRight;
	plotHeight = h - padTop - padBottom;
	xBox = plotWidth/4;
	yBox = (plotHeight)/2;
}
void TetrodePlot::movePlot(int x, int y){
	xPos = x;
	yPos = y;
}
void TetrodePlot::draw()
{
//	std::cout<<"TetrodePlot::draw()"<<std::endl;
	// if(disableWaveOverlay)
	// 	eraseWaveforms();
		
	// highlightSelectedWaveform();
	// Get the new spikes in the queue and plot them


	do{
		drawWaveforms();
		drawProjections();
		if (newSpike)
			addSpikeToVBO();
	}while(newSpike = tryToGetSpikeForPlotting(&spike));

	drawTitle();
	drawBoundingBoxes();
	
}
void TetrodePlot::addSpikeToVBO(){

	
	int newIdx = countInVBO % MAX_N_PROJ_POINTS;
	

	// if (countInVBO%500==0){
	// 	printf("Adding new spike to vbo: %d %d %d %d\n", sv.x, sv.y, sv.z, sv.w);
	// 	printf("Drawing %d spikes in the VBO\n", newIdx);
	// }
	SpikeVertex *vertexToAdd = &sv;
// 	vertexToAdd = &tmp;
	sv.z = 0;
	sv.w = 1;

	glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBufferSubData(GL_ARRAY_BUFFER, newIdx*sizeof(SpikeVertex), sizeof(SpikeVertex), vertexToAdd);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	countInVBO++;
	newSpike = false;
	
}
void TetrodePlot::highlightSelectedWaveform(){
//	setViewportForWaveN(selectedWaveform);
//	glColor3f(colSelected[0], colSelected[1], colSelected[2]);
//	glRecti(-1, -1, 2, 2);
}

void TetrodePlot::drawWaveforms(){
	glLineWidth(TetrodePlot::waveformLineWidth);
	for (int i=0; i<nChan; i++) 
			drawWaveformN(i);
}


void TetrodePlot::drawWaveformN(int n)
{
	setViewportForWaveN(n);
	
	// Disp the threshold value


	// Draw the actual waveform
	float dx = 2.0/(spike.n_samps_per_chan-1);
	float x = -1;
	int	sampIdx = n; 
	glColor3f(colWave[0], colWave[1], colWave[2]);
	
	glBegin( GL_LINE_STRIP );
	for (int i=0; i<spike.n_samps_per_chan; i++)
	{
		glVertex2f(x, spike.data[sampIdx]);
		sampIdx +=4;
		x +=dx;
	}
	glEnd();

	// Draw the threshold line
	int thresh = spike.thresh[n];
	glColor3f(colThres[0], colThres[1], colThres[2]); // set threshold line to red
	glLineStipple(4, 0xAAAA); // make a dashed line
	glEnable(GL_LINE_STIPPLE);
	glBegin( GL_LINE_STRIP );
		glVertex2f(-1.0, thresh);
		glVertex2f( 1.0, thresh);
	glEnd();		
	glDisable(GL_LINE_STIPPLE);
	
	
}
void TetrodePlot::drawProjections(){

	int maxIdx = calcWaveMaxInd();

	maxIdx = maxIdx  - maxIdx%4;  

	sv.x = spike.data[maxIdx];
	sv.y = spike.data[maxIdx+1];
	sv.z = spike.data[maxIdx+2];
	sv.w = spike.data[maxIdx+3];
	
	for (int i=0; i<nProj; i++)
		drawProjectionN(i);
	
}


void TetrodePlot::drawProjectionN(int n){

	setViewportForProjectionN(n);
	// drawViewportCross(MIN_VOLT,MIN_VOLT,MAX_VOLT,MAX_VOLT);
	

	int d1, d2;
	if (n==0){
		d1 = sv.x;
		d2 = sv.y;
	}
	else if(n==1){
		d1 = sv.x;
		d2 = sv.z;
	}
	else if(n==2){
		d1 = sv.x;
		d2 = sv.w;
	}
	else if(n==3){
		d1 = sv.y;
		d2 = sv.z;
	}
	else if(n==4){
		d1 = sv.y;
		d2 = sv.w;
	}
	else if (n==5){
		d1 = sv.z;
		d2 = sv.w;
	}
	else{
		d1 = sv.x;
		d2 = sv.y;
		std::cout<<"Invalid projection combination selected, please restrict value to 0:5"<<std::endl;
	}

	glColor3f( 1.0, 1.0, 1.0);
	drawProjectionVBO(n);

	if (newSpike){
		glColor3f( 1.0, 0.0, 0.0 );
		glPointSize(5);
		glBegin(GL_POINTS);
			glVertex2f(d1, d2);
			//glVertex2f(0,0);
		glEnd();
		glPointSize(1);
	}
	
//	if(countInVBO%500==0 and n == 0)
//		printf("Plotting new spike:%d,%d\n",d1,d2);
}

void TetrodePlot::drawProjectionVBO(int plotAxes){

//	std::cout<<"drawingProjectionVBO"<<std::endl;
	int nPlot = (countInVBO > MAX_N_PROJ_POINTS) ? MAX_N_PROJ_POINTS:countInVBO;


	glColor3f(1.0,1.0,1.0);
//	GLint axes = glGetUniformLocation(shaderProg, "axes"); 
//	glUniform1i(axes, plotAxes);
	// Draw the VBO
	
	glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_INT, sizeof(SpikeVertex), NULL);
	
	glDrawElements(GL_POINTS, nPlot, GL_UNSIGNED_INT, NULL);
	glDisableClientState(GL_VERTEX_ARRAY);

//	std::cout<<"Drawing VBO nPlot:"<<nPlot<<std::endl;


//	glUseProgram(0);	//Disable the shader program

}
void TetrodePlot::drawTitle(){
	setViewportForTitleBox();
	if (!isSelected)
		glColor3f(colTitle[0], colTitle[1], colTitle[2]);
	else
		glColor3f(colTitleSelected[0], colTitleSelected[1], colTitleSelected[2]);
	glRectf(-1,-1,2,2);
	glColor3f(1.0, 1.0, 1.0);
	
	char num[]="00";
	if (tetrodeNumber<10)
		sprintf(num,"%d%d",0,tetrodeNumber);
	else
		sprintf(num,"%d",tetrodeNumber);
		
	char titleString[] = "TT:00 on Port:____";
	memcpy(titleString+3, num, 2);
	memcpy(titleString+14, port, 4);
	
	double yScale = 2.0/titleHeight;
	double yOffset = -1*11.0/2*yScale;
	
	double xScale = 2.0/(4*xBox);
	double xOffset = -1*(9.0 * strlen(titleString))/2*xScale;

	if(xOffset<-1)
		xOffset = -1;
	if(yOffset<-1)
		yOffset = -1;

	drawString(xOffset,yOffset, titleFont, titleString);
}


void TetrodePlot::eraseWaveforms(){

	for (int i=0; i<4; i++){
		setViewportForWaveN(i);
		glColor3f(0,0,0);
		glRectf(-1, -1, 2, 2);
	}
}


void TetrodePlot::setViewportForTitleBox(){

//	std::cout<<"Setting viewport for title box"<<<std::endl;

	float viewDX = xBox*4;
	float viewDY = titleHeight;
	float viewX = padLeft;
	float viewY = 2*yBox - titleHeight - padBottom;
	
//	glViewport(viewX+xPos,viewY+yPos,viewDX,viewDY);
	setViewportWithRange(viewX+xPos, viewY+yPos, viewDX, viewDY, 0, 0, 1, 1);
}
void TetrodePlot::setViewportForWaveN(int n){
	float viewDX = xBox/2;
	float viewDY = yBox - titleHeight/2;
	float viewX,viewY;
	switch (n){
	case 0:
		viewX=0;
		viewY=yBox - titleHeight/2 - padBottom;
		break;
	case 1:
		viewX = xBox/2;
		viewY = yBox - titleHeight/2 - padBottom;
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
	viewX = viewX + padLeft;
	viewY = viewY + padBottom;
	viewDX = viewDX + padLeft;
	viewDY = viewDY - padBottom;
	
	setViewportWithRange(viewX+xPos, viewY+yPos, viewDX, viewDY, -1, minAmpRange, 1, maxAmpRange);	
}


void TetrodePlot::setViewportForProjectionN(int n){
//	std::cout<<"Setting viewport on projection:"<<n<<std::endl;
    float viewDX = xBox;
    float viewDY = yBox - titleHeight/2;
    float viewX,viewY;

    switch (n){
    case 0:
        viewX=xBox;
        viewY=yBox-titleHeight/2 - padBottom;
        break;
    case 1:
        viewX = xBox*2;
        viewY = yBox-titleHeight/2 - padBottom;
        break;
    case 2:
        viewX=xBox*3;
        viewY=yBox-titleHeight/2 - padBottom;
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
	viewX = viewX + padLeft;
	viewY = viewY + padBottom;
	viewDX = viewDX;
	viewDY = viewDY - padBottom;

	setViewportWithRange(viewX+xPos, viewY+yPos, viewDX, viewDY, minAmpRange, minAmpRange, maxAmpRange, maxAmpRange);
}

int TetrodePlot::incrementIdx(int i){
	i++;
	if (i>=MAX_SPIKE_BUFF_SIZE)
		i = 0;
	return i;
}

int  TetrodePlot::calcWaveMaxInd(){

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



void TetrodePlot::drawBoundingBoxes(void){

	
	glColor3f(1.0, 1.0, 1.0);
	
	setViewportForTitleBox();
	drawViewportEdge();
	
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

//void TetrodePlot::drawViewportEdge(){
//}


void TetrodePlot::resetSeqNum()
{
//    curSeqNum = 0;
//    clearWindow();
    std::cout<<"Reseting sequence number to 0!"<<std::endl;
}

bool TetrodePlot::tryToGetSpikeForPlotting(spike_net_t *s){
	
	if  (readIdx==writeIdx || nSpikes==0)
	{
		return false;
	}
	
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

/*
void TetrodePlot::drawString(float x, float y, void *f, char *string){

	glRasterPos2f(x, y);

//	string = "123";
	int len = strlen(string);
	for (int i = 0; i < len; i++) {
   	glutBitmapCharacter(f, string[i]);
	}
}
*/

void TetrodePlot::initNetworkRxThread(){
	pthread_t netThread;
	net = NetCom::initUdpRx(host,port);
	pthread_create(&netThread, NULL, networkThreadFunc, this);
}

void TetrodePlot::getNetworkSpikePacket(){
	while(true){
		
		spike_net_t s;
		NetCom::rxSpike(net, &s);
		spikeBuff[writeIdx] = s;
		
		writeIdx = incrementIdx(writeIdx);
		nSpikes++;
	}
}

void *networkThreadFunc(void *ptr){
//	std::cout<<"NetworkThreadFunc called!"<<std::endl;
//	std::cout<< ptr << " Address of pointer coming into networkThreadFunc\n";
	// TetrodePlot *tp = reinterpret_cast<TetrodePlot *>(ptr);
	TetrodePlot *tp = (TetrodePlot*) ptr;
	tp->getNetworkSpikePacket();
}



void TetrodePlot::setSelected(bool s){
	isSelected = s;
}
bool TetrodePlot::isPlotSelected(){
	return isSelected;
}

void TetrodePlot::scaleUp(){
	userScale += dUserScale;
}
void TetrodePlot::scaleDown(){
	userScale -= dUserScale;	
}
void TetrodePlot::setScale(float s){
	userScale = s;
}
float TetrodePlot::getScale(){
	return userScale;
}
void TetrodePlot::shiftUp(){
	userShift += dUserShift;
}
void TetrodePlot::shiftDown(){
	userShift -= dUserShift;
}
void TetrodePlot::setShift(float s){
	userShift = s;
}
float TetrodePlot::getShift(){
	return userShift;
}
void TetrodePlot::setWaveformOverlay(bool o){
	disableWaveOverlay = o;
}
bool TetrodePlot::toggleWaveformOverlay(){
	disableWaveOverlay = !disableWaveOverlay;
}
bool TetrodePlot::getWaveformOverlay(){
	return disableWaveOverlay;
}

void TetrodePlot::clearPlot(){
	printf("Clearing tetrode plot%d\n", tetrodeNumber);
	for (int i=0; i<nProj; i++)
	{
		setViewportForProjectionN(i);
		glColor3f(0,0,0);
		glRecti(-1,-1,2,2);
	}
	countInVBO=0;
		
//	clearProjectionNextPlot = true;
}
int TetrodePlot::getTetrodeNumber(){
	return tetrodeNumber;
}
void TetrodePlot::setTetrodeNumber(int n){
	tetrodeNumber = n;
}

bool TetrodePlot::containsPoint(int x, int y){
	return ((xPos < x && xPos + plotWidth > x) && (yPos<y && yPos+plotHeight > y));
}

int TetrodePlot::getMaxX(){
	return xPos+xBox*4;
}
int TetrodePlot::getMinX(){
	return xPos;
}
	
void TetrodePlot::setShaderProgram(GLuint p){
	shaderProg = p;
}

