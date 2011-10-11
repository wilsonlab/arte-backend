#define GL_GLEXT_PROTOTYPES
	#include <GLUT/glut.h>
	#include <OpenGL/gl.h> //OS x libs
	#include <OpenGL/glu.h>
	#include <OpenGL/glext.h>

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <time.h>

static const int nChan = 4;
static const int MAX_N_POINT = 1e2;

GLint data1[MAX_N_POINT][nChan]; // XYZW
GLint data2[MAX_N_POINT][nChan]; // YWXZ
GLint data3[2][MAX_N_POINT];

struct ClusterVertex{
	int x, y, z, w;
};


inline int getDimIdx(int dim, int arrayN){
	if (arrayN==0)
		return dim;
	else
		if(dim==0)
			return 2;
		if(dim==1)
			return 0;
		if(dim==2)
			return 3;
		if(dim==3)
			return 1;
		return -1;
}

void drawPoints(){

	int idx = 0;
	int nPlot = 100;

	for (int j=0; j<MAX_N_POINT; j++)
	{		
		data1[j][getDimIdx(0,0)] = 200 + rand()%200 - 100;
		data1[j][getDimIdx(1,0)] = 400 + rand()%200 - 100;
		data1[j][getDimIdx(2,0)] = 600 + rand()%200 - 100;
		data1[j][getDimIdx(3,0)] = 800 + rand()%200 - 100; //val;

		data2[j][getDimIdx(0,1)] = 200 + rand()%200 - 100;
		data2[j][getDimIdx(1,1)] = 400 + rand()%200 - 100;
		data2[j][getDimIdx(2,1)] = 600 + rand()%200 - 100;
		data2[j][getDimIdx(3,1)] = 800 + rand()%200 - 100; //val;
	}

	glEnableClientState(GL_VERTEX_ARRAY);


	glColor3f(1.0, 1.0, 0.0);
	glVertexPointer(2, GL_INT, 4*sizeof(GLint), &data1[0][0]);
	glDrawArrays(GL_POINTS, 0, MAX_N_POINT-1);

	glColor3f(0.0, 1.0, 1.0);
	glVertexPointer(2, GL_INT, 4*sizeof(GLint), &data1[0][1]);
	glDrawArrays(GL_POINTS, 0, MAX_N_POINT-1);
	
	glColor3f(1.0, 0.0, 1.0);
	glVertexPointer(2, GL_INT, 4*sizeof(GLint), &data1[0][2]);
	glDrawArrays(GL_POINTS, 0, MAX_N_POINT-1);

	glColor3f(1.0, 0.0, 0.0);
	glVertexPointer(2, GL_INT, 4*sizeof(GLint), &data2[0][0]);
	glDrawArrays(GL_POINTS, 0, MAX_N_POINT-1);
	
	glColor3f(0.0, 1.0, 0.0);
	glVertexPointer(2, GL_INT, 4*sizeof(GLint), &data2[0][1]);
	glDrawArrays(GL_POINTS, 0, MAX_N_POINT-1);
	
	glColor3f(0.0, 0.0, 1.0);
	glVertexPointer(2, GL_INT, 4*sizeof(GLint), &data2[0][2]);
	glDrawArrays(GL_POINTS, 0, MAX_N_POINT-1);



	glDisableClientState(GL_VERTEX_ARRAY);	

}

void draw(){
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.2, 0.2);
	glRectf(200,200,400,400);
	
	glColor3f(0.2, 0.2, 0.0);
	glRectf(400,400,600,600);
	 
	glColor3f(0.2, 0.0, 0.2);
	glRectf(600,600,800,800);
	
	drawPoints();
	
	glutSwapBuffers();
	glFlush();
	
	
}
void idle(){
	draw();
	usleep(1e6/1000);
}
void resize(int h, int w){
	glViewport(0,0,h,w);
	glTranslatef(-1.0, -1.0, 0.0);
	glScalef(2.0/1000.0, 2.0/1000.0, 0);
}
int initVBO(){
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER,buffer);
}
int main(int argc, char* argv[]){
	
	srand ( time(NULL) );
  
	bzero(data2, sizeof(GLint)*nChan*MAX_N_POINT);
	int val=0;

	printf("First 4 data points: %d %d %d %d \n", data2[0][0], data2[0][1], data2[0][2], data2[0][3]);
		
	glutInit(&argc,argv);
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowPosition( 5, 20);
	glutInitWindowSize(300, 300);
	glutCreateWindow("OpenGL Swizzle Test on 4D Points");

	glutReshapeFunc( resize );
	glutIdleFunc( idle );
	glutDisplayFunc( draw );
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glutMainLoop( );

	return(0);
}

/* A simple function that will read a file into an allocated char pointer buffer */
char* filetobuf(char *file)
{
    FILE *fptr;
    long length;
    char *buf;
 
    fptr = fopen(file, "rb"); /* Open file for reading */
    if (!fptr) /* Return NULL on failure */
        return NULL;
    fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
    length = ftell(fptr); /* Find out how many bytes into the file we are */
    buf = (char*)malloc(length+1); /* Allocate a buffer for the entire length of the file and a null terminator */
    fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
    fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
    fclose(fptr); /* Close the file */
    buf[length] = 0; /* Null terminator */
 
    return buf; /* Return the buffer */
}