#include "PlotUtils.h"

void drawString(float x, float y, void *f, char *string){

	glRasterPos2f(x, y);
//	string = "123";
	int len = strlen(string);
	for (int i = 0; i < len; i++) {
//   	glutBitmapCharacter(f, string[i]);
	}
}
void drawViewportEdge(){
	glBegin(GL_LINE_LOOP);
		glVertex2f(-.995, -.995);
		glVertex2f( .995, -.995);
		glVertex2f( .995, .995);
		glVertex2f(-.995, .995);
	glEnd();
}
