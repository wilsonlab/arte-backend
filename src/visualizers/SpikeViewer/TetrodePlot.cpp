
#include "TetrodePlot.h"

TetrodePlot::TetrodePlot(){
	
}
TetrodePlot::TetrodePlot(int x, int y, int w, int h, char *p){
	std::cout<<"Initializing a new TetrodePlot\n"<<std::flush;
	xPos = x, yPos = y;
	plotWidth = w, plotHeight = h;
	
	plotPadX = 2;
	plotPadY = 2;
	resizePlot(plotWidth, plotHeight);

	disableWaveOverlay = true;
	waveformLineWidth = 1;
	memcpy(host, "127.0.0.1", 10);
	host[10] = 0;
	port = p;

	// ===================================
	// 		Scaling Variables
	// ===================================
	xRange = MAX_INT;
	yRange = MAX_INT;

	xScale = 2.0/(double)xRange;
	yScale = 2.0/(double)yRange;

	dV = 1.0/((float)MAX_VOLT*2);
	userScale = 1;
	dUserScale = .3;

	voltShift = -.85;
	userShift = 0;
	dUserShift = .05;

	nChan=4;
	nProj=6;
	
//	bzero(&spikeBuff, sizeof(spikeBuff));
	spikeBuff = new spike_net_t[MAX_SPIKE_BUFF_SIZE];
	nSpikes = 0;
	readIdx = 0;
	writeIdx = 0;
	curSeqNum = 0;
	
	initNetworkRxThread();

}
void TetrodePlot::resizePlot(int w, int h)
{
	plotWidth = w - plotPadX*2;
	plotHeight = h - plotPadY*2;
	xBox = plotWidth/4;
	yBox = plotHeight/2;
}
void TetrodePlot::movePlot(int x, int y){
	xPos = x+plotPadX;
	yPos = y+plotPadY;
}
void TetrodePlot::draw()
{
	eraseWaveforms();
	highlightSelectedWaveform();

	// Get the new spikes in the queue and plot them
	do{
		drawWaveforms();
		drawProjections();
	}while(tryToGetSpikeForPlotting(&spike));

	drawTitle();
	drawBoundingBoxes();
	
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
	int thresh = spike.thresh[n];
//	sprintf(txtDispBuff, "T:%d", thresh);
//	glColor3f(colFont[0],colFont[1],colFont[2]);
//	drawString(-.9, .8, txtDispBuff);

	// Draw the actual waveform
	float dx = 2.0/(spike.n_samps_per_chan-1);
	float x = -1;
	int	sampIdx = n; 
	glColor3f(colWave[0], colWave[1], colWave[2]);
	glBegin( GL_LINE_STRIP );
	for (int i=0; i<spike.n_samps_per_chan; i++)
	{
		glVertex2f(x, scaleVoltage(spike.data[sampIdx], true));
		sampIdx +=4;
		x +=dx;
	}
	glEnd();

	// Draw the threshold line
	glColor3f(colThres[0], colThres[1], colThres[2]); // set threshold line to red
	glLineStipple(4, 0xAAAA); // make line a dashed line
	glEnable(GL_LINE_STIPPLE);
	glBegin( GL_LINE_STRIP );
		glVertex2f(-1.0, scaleVoltage(thresh, true));
		glVertex2f( 1.0, scaleVoltage(thresh, true));
	glEnd();		
	glDisable(GL_LINE_STIPPLE);
	
}
void TetrodePlot::drawProjections(){

	int maxIdx = calcWaveMaxInd();

	maxIdx = maxIdx  - maxIdx%4;  

	for (int i=0; i<nProj; i++) // <----------------------------------------
		drawProjectionN(i, maxIdx);

}


void TetrodePlot::drawProjectionN(int n, int idx){

	setViewportForProjectionN(n);

	int d1, d2;
	if (n==0){
		d1 = 0;
		d2 = 1;
	}
	else if(n==1){
		d1 = 0;
		d2 = 2;
	}
	else if(n==2){
		d1 = 0;
		d2 = 3;
	}
	else if(n==3){
		d1 = 1;
		d2 = 2;
	}
	else if(n==4){
		d1 = 1;
		d2 = 3;
	}
	else if (n==5){
		d1 = 2;
		d2 = 3;
	}
	else{
		d1 = 0;
		d2 = 1;
		std::cout<<"Invalid projection combination selected, please restrict value to 0:5"<<std::endl;
	}

	glColor3f( 1.0, 1.0, 1.0 );
	glBegin(GL_POINTS);
//		glVertex2f(scaleVoltage(spike.data[idx+d1], false), scaleVoltage(spike.data[idx+d2], false));
		glVertex2f(0,0);
	glEnd();
}
void TetrodePlot::drawTitle(){
	setViewportForTitleBox();
//	drawString(0,0,"Title Box");
}


void TetrodePlot::eraseWaveforms(){

	glViewport(xPos, yPos, xBox, yBox*2 - yPadding);

	glColor3f(0,0,0);
	glRectf(-1, -1, 2, 2);

}


void TetrodePlot::setViewportForTitleBox(){

//	std::cout<<"Setting viewport for title box"<<<std::endl;

	float viewDX = xBox*4;
	float viewDY = titleHeight;
	float viewX = 0;
	float viewY = plotHeight-titleHeight;//yBox*2 - titleHeight;
	glViewport(viewX+xPos,viewY+yPos,viewDX,viewDY);
}
void TetrodePlot::setViewportForWaveN(int n){
	float viewDX = xBox/2;
	float viewDY = yBox - titleHeight/2;
	float viewX,viewY;
	switch (n){
	case 0:
		viewX=0;
		viewY=yBox - titleHeight/2;
		break;
	case 1:
		viewX = xBox/2;
		viewY = yBox - titleHeight/2;
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
	viewX = viewX + xPadding;
	viewY = viewY + yPadding;
	viewDX = viewDX - 2*xPadding;
	viewDY = viewDY - 2*yPadding;
	
	glViewport(viewX+xPos,viewY+yPos,viewDX,viewDY);
}


void TetrodePlot::setViewportForProjectionN(int n){
//	std::cout<<"Setting viewport on projection:"<<n<<std::endl;
    float viewDX = xBox;
    float viewDY = yBox - titleHeight/2;
    float viewX,viewY;

    switch (n){
    case 0:
        viewX=xBox;
        viewY=yBox -titleHeight/2;
        break;
    case 1:
        viewX = xBox*2;
        viewY = yBox-titleHeight/2;
        break;
    case 2:
        viewX=xBox*3;
        viewY=yBox-titleHeight/2;
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
	viewX = viewX + xPadding;
	viewY = viewY + yPadding;
	viewDX = viewDX - 2*xPadding;
	viewDY = viewDY - 2*yPadding;


	glViewport(viewX+xPos, viewY+yPos, viewDX, viewDY);
}

/*
void drawProjections(){

	int maxIdx = calcWaveMaxInd();

	maxIdx = maxIdx  - maxIdx%4;  

	for (int i=0; i<nProj; i++) // <----------------------------------------
		drawProjectionN(i, maxIdx);

}
void drawProjectionN(int n, int idx){
*/
int  TetrodePlot::calcWaveMaxInd(){
/*
	int idx = -1;
	int val = -1*2^15;
	for (int i=0; i<spike.n_samps_per_chan * spike.n_chans; i++)
		if(val < spike.data[i])
		{
			idx = i;
			val = spike.data[i];
		}
	return idx;*/
	return 0;
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

void TetrodePlot::drawViewportEdge(){
	glBegin(GL_LINE_LOOP);
		glVertex2f(-.999, -.999);
		glVertex2f( .999, -.999);
		glVertex2f( .999, .999);
		glVertex2f(-.999, .999);
	glEnd();
}


void TetrodePlot::resetSeqNum()
{
//    curSeqNum = 0;
//    clearWindow();
    std::cout<<"Reseting sequence number to 0!"<<std::endl;
}


int TetrodePlot::incrementIdx(int i){
/*	if(i==MAX_SPIKE_BUFF_SIZE-1)
		return 0;
	else
		return i+1;*/
		return (i+1)%MAX_SPIKE_BUFF_SIZE;

}
bool TetrodePlot::tryToGetSpikeForPlotting(spike_net_t *s){
	std::cout<<"ReadIDX:"<<readIdx<< " nSpikes:"<<nSpikes<<std::endl<<std::flush;
	
	if  (readIdx==writeIdx || nSpikes==0)
		return false;

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

float TetrodePlot::scaleVoltage(int v, bool shift){
	if (shift)
		return ((float)v * dV * userScale) + voltShift + userShift;
	else
		return ((float)v * dV * userScale) + voltShift;
}

void TetrodePlot::drawString(float x, float y, char *string){
/*
	glRasterPos2f(x, y);

	int len = strlen(string);
	for (int i = 0; i < len; i++) {
    	glutBitmapCharacter(font, string[i]);
	}*/
}

void TetrodePlot::initNetworkRxThread(){
	pthread_t netThread;
	net = NetCom::initUdpRx(host,port);
	pthread_create(&netThread, NULL, networkThreadFunc, this);
}

void TetrodePlot::getNetworkSpikePacket(){
	

	spike_net_t s;
	NetCom::rxSpike(net, &s);
	printf("got network spike!\n");
	spikeBuff[writeIdx] = s;
	printf("spike written to buffer\n");
	writeIdx = incrementIdx(writeIdx);
	nSpikes+=1;
	totalSpikesRead++; 
	printf("writeIdx:%d nspikes:%d totSpike:%d", writeIdx, nSpikes, totalSpikesRead);
}

void *networkThreadFunc(void *ptr){
	std::cout<<"NetworkThreadFunc called!"<<std::endl;
	TetrodePlot *tp = reinterpret_cast<TetrodePlot *>(ptr);

	while(true)
	{
		tp->getNetworkSpikePacket();
	}
}



