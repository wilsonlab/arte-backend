#include "PlotUtils.h"


void checkGlError(){
	GLenum errCode;
	const GLubyte *errString;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
	    errString = gluErrorString(errCode);
	   	fprintf (stderr, "OpenGL Error: %s\n", errString);
		exit(1);
	}
	else
		std::cout<<"OpenGL Okay!"<<std::endl;
}
void drawString(float x, float y, void *f, char *string){

	glRasterPos2f(x, y);
//	string = "123";
	int len = strlen(string);
	for (int i = 0; i < len; i++) {
   	glutBitmapCharacter(f, string[i]);
	}
}
void drawViewportEdge(){
	glPushMatrix();
	glLoadIdentity();
	
	glBegin(GL_LINE_LOOP);
		glVertex2f(-.995, -.995);
		glVertex2f( .995, -.995);
		glVertex2f( .995, .995);
		glVertex2f(-.995, .995);
	glEnd();
	
	glPopMatrix();
}

void drawViewportCross(int xMin, int yMin, int xMax, int yMax){
	glColor3f(0.0,0.4,0.0);

	glBegin(GL_LINE_STRIP);
	glVertex2i(xMin,yMin);
	glVertex2i(xMax,yMax);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2i(xMax,yMin);
	glVertex2i(xMin, yMax);
	glEnd();
}

void setViewportWithRange(int x,int y,int w,int h,int xMin,int yMin,int xMax,int yMax){
	
	float dx = xMax-xMin;
	float dy = yMax-yMin;
	
//	printf("Setting viewport to:%d,%d %d,%d with dims%d,%d %d,%d\n", x,y,w,h, xMin, xMin, xMax,yMax);
//	printf("Dx:%f Dy:%f, Scaling viewport by:%f,%f \n", dx,dy,2.0/dx, 2.0/dy);
	glViewport(x,y,w,h);
	glLoadIdentity();
	glTranslatef(-1.0,-1.0,0.0); 
	glScalef(2.0/dx, 2.0/dy, 1.0);
	glTranslatef(0-xMin, 0-yMin, 0);
	
//	glScalef(0.5,0.5,1); 


//	glScalef(1.0f/dx, 1.0/dy, 1.0);
	
//	glTranslatef(0-xMin, 0-yMin, 0);

}