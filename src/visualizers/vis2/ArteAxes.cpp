#include "ArteAxes.h"

ArteAxes::ArteAxes():
ArteUIElement(),
type(1)
{	
	xlims[0] = 0;
	xlims[1] = 1;
	ylims[0] = 0;
	ylims[1] = 0;
}

ArteAxes::ArteAxes(int x, int y, double w, double h, int t):
ArteUIElement(x,y,w,h)
{
	// if (t<WAVE1 || t>PROJ3x4)
		//("Invalid Axes type specified");
	type = t;
	setWaveformColor(1.0,1.0,0.6);
	setThresholdColor(1.0, 0.1, 0.1);
	setPointColor(1.0, 1.0, 1.0);
}
void ArteAxes::updateSpikeData(spike_net_t *newSpike){
	s = newSpike;
}
void ArteAxes::redraw(){
	ArteUIElement::redraw();
	if (ArteUIElement::enabled)
		plotData();
	ArteUIElement::drawElementEdges();
}
void ArteAxes::plotData(){
	switch(type){
		case WAVE1:
		case WAVE2:
		case WAVE3:
		case WAVE4:
		plotWaveform(type);
		break;
		
		case PROJ1x2:
		case PROJ1x3:
		case PROJ1x4:
		case PROJ2x3:
		case PROJ2x4:
		case PROJ3x4:
		plotProjection(type);
		break;
		// default:
		// error("ArteAxes::plotData(), Invalid type specified, cannot be plotted");
	}
}
void ArteAxes::setXLims(double xmin, double xmax){
	// if (xmin>=ymin)
		// error("ArteAxes::setXLims xmin must be less than xmax");
	xlims[0] = xmin;
	xlims[1] = xmax;
}
void ArteAxes::setYLims(double ymin, double ymax){
	// if (ymin>=ymin)
		// error("ArteAxes::setYLims ymin must be less than ymax");
	xlims[0] = ymin;
	ylims[1] = ymax;
}
void ArteAxes::setType(int t){
	// if (t<WAVE1 || t>PROJ3x4)
		// error("Invalid Axes type specified");
	type = t;
}


void ArteAxes::plotWaveform(int chan){

	// if (chan>WAVE4 || chan<WAVE1)
		// error("ArteAxes::plotWaveform() invalid channel, must be between 0 and 4");
		
	//compute the spatial width for each wawveform sample	
	float dx = (xlims[1]-xlims[0]) / (s->n_samps_per_chan-1);
	float x = -1;
	int	sampIdx = chan; 

	//Draw the individual waveform points connected with a line
	// if drawWaveformPoints is set to false then force the drawing of the line, _SOMETHING_ must be drawn
	if(drawWaveformLine || !drawWaveformPoints){
		glColor3fv(waveColor);
		glBegin( GL_LINE_STRIP );
		for (int i=0; i<s->n_samps_per_chan; i++)
		{
			glVertex2f(x, s->data[sampIdx]);
			sampIdx +=4;
			x +=dx;
		}
		glEnd();
	}
	
	if(drawWaveformPoints){
		x = -1;
		glColor3fv(pointColor);
		glPointSize(5);
		glBegin( GL_LINE_STRIP );
		for (int i=0; i<s->n_samps_per_chan; i++)
		{
			glVertex2f(x, s->data[sampIdx]);
			sampIdx +=4;
			x +=dx;
		}
		glEnd();
	}
	// Draw the threshold line
	int thresh = s->thresh[chan];
	
	glColor3fv(thresholdColor); 
	glLineStipple(4, 0xAAAA); // make a dashed line
	glEnable(GL_LINE_STIPPLE);
	glBegin( GL_LINE_STRIP );
		glVertex2f(xlims[0], thresh);
		glVertex2f(xlims[1], thresh);
	glEnd();		
	glDisable(GL_LINE_STIPPLE);
}


void ArteAxes::plotProjection(int proj){
	// if (proj<PROJ1x2 || proj>PROJ3x4)
		// error("ArteAxes:plotProjection() invalid projection specified");
		
	int d1, d2;
	if (proj==PROJ1x2){
		d1 = 0;
		d2 = 1;
	}
	else if(proj==PROJ1x3){
		d1 = 0;
		d2 = 2;
	}
	else if(proj==PROJ1x4){
		d1 = 0;
		d2 = 3;
	}
	else if(proj==PROJ2x3){
		d1 = 1;
		d2 = 2;
	}
	else if(proj==PROJ2x4){
		d1 = 1;
		d2 = 3;
	}
	else if (proj==PROJ3x4){
		d1 = 2;
		d2 = 3;
	}
	else{
		// error("ArteAxes::plotProjection() invalid projection specified cannot determine d1 and d2");
	}
	
	int maxIdx = calcWaveformPeakIdx();
	
	glColor3fv(pointColor);
	glPointSize(1);
	glBegin(GL_POINTS);
		glVertex2f(s->data[maxIdx+d1], s->data[maxIdx+d2]);
	glEnd();
	
}

int ArteAxes::calcWaveformPeakIdx(){
//Calculate which sample in the waveform across all channels has the highest peak voltage 
//and then calculate its sample number

	int idx = -1;
	int val = -1*2^15;
	for (int i=0; i<s->n_samps_per_chan * s->n_chans; i++)
		if(val < s->data[i])
		{
			idx = i;
			val = s->data[i];
		}
	// The index of the peak voltage can be any of the channels so shift it back to the first channel
	idx = idx - idx%s->n_chans;
	return idx;
}
void ArteAxes::setWaveformColor(GLfloat r, GLfloat g, GLfloat b){
	waveColor[0] = r;
	waveColor[1] = g;
	waveColor[2] = b;
}
void ArteAxes::setThresholdColor(GLfloat r, GLfloat g, GLfloat b){
	thresholdColor[0] = r;
	thresholdColor[1] = g;
	thresholdColor[2] = b;
}
void ArteAxes::setPointColor(GLfloat r, GLfloat g, GLfloat b){
	pointColor[0] = r;
	pointColor[1] = g;
	pointColor[2] = b;
}