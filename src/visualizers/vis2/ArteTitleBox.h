#ifndef ARTE_TITLE_BOX_H
#define ARTE_TITLE_BOX_H

#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif

#include "ArteUIElement.h"


class ArteTitleBox : public ArteUIElement{
	
	char *title;
	bool selected;
	void drawTitle();
	
	GLfloat titleColor[3];
	GLfloat selectedColor[3];
	
public:
	ArteTitleBox();
	ArteTitleBox(int x, int y,int w,int h, char *n);

	void redraw();
	void setTitle(char *n);
	void setSelected(bool sel);
	void setColor(float, float, float);
	void setSelectedColor(float, float, float);
};


#endif
