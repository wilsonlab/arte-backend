#define GL_GLEXT_PROTOTYPES
#include <glew.h> // Include the GLEW header file

#include <GLUT/glut.h>
#include <OpenGL/gl.h> //OS x libs
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
	
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <time.h>

#include "shaderutils.h"

static const int nChan = 4;
static const int MAX_N_POINT = 4e5;

GLint data[MAX_N_POINT][nChan]; // XYZW


GLuint axesShader;
GLuint shaderProg;

struct ClusterVertex{
	int x, y, z, w;
};

void setColor(int i){
	switch(i){
		case 0:
			glColor3f(1.0, 0.0, 0.0);
			break;
		case 1:
			glColor3f(0.0, 1.0, 0.0);
			break;
		case 2:
			glColor3f(0.0, 0.0, 1.0);
			break;
		case 3:
			glColor3f(1.0, 1.0, 0.0);
			break;
		case 4: 
			glColor3f(0.0, 1.0, 1.0);
			break;
		case 5:
			glColor3f(1.0, 0.0, 1.0);
			break;
		default:
			glColor3f(1.0, 1.0, 1.0);
	}
}
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
void checkGlError(){
	GLenum errCode;
	const GLubyte *errString;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
	    errString = gluErrorString(errCode);
	   fprintf (stderr, "OpenGL Error: %s\n", errString);
	}
}
void initPoints()
{
	for (int j=0; j<MAX_N_POINT; j++)
	{		
		data[j][getDimIdx(0,0)] = 000 + rand()%200;
		data[j][getDimIdx(1,0)] = 200 + rand()%200;
		data[j][getDimIdx(2,0)] = 400 + rand()%200;
		data[j][getDimIdx(3,0)] = 600 + rand()%200; //val;
	}
}
void drawPoints(){

	int idx = 0;
	int nPlot = 100;

	

	glColor3f(1.0, 1.0, 0.0);

	glVertexPointer(4, GL_INT, 4*sizeof(GLint), &data[0][0]);
	
	//New shader code here
	for (int i=0; i<6; i++){
		setColor(i);
		glUseProgram(shaderProg);
		int plotAxes = i;
		GLint axes = glGetUniformLocation(shaderProg, "axes"); 
	  	glUniform1i(axes, plotAxes);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays(GL_POINTS, 0, MAX_N_POINT-1);
		glDisableClientState(GL_VERTEX_ARRAY);	
		glUseProgram(0);
	}
	
	/*

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

	*/

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
	
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_LINE_LOOP);
		glVertex2i(1,1);
		glVertex2i(1,1000);
		glVertex2i(1000,1000);
		glVertex2i(1000,1);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex2i(  -1,200);
		glVertex2i(1001,200);
		glVertex2i(1001,400);
		glVertex2i(  -1,400);
		glVertex2i(  -1,600);
		glVertex2i(1001,600);
		glVertex2i(1001,800);
		glVertex2i(  -1,800);
	glEnd();
	
	glBegin(GL_LINE_STRIP);
		glVertex2i(200,  -1);
		glVertex2i(200,1001);
		glVertex2i(400,1001);
		glVertex2i(400,  -1);
		glVertex2i(600,  -1);
		glVertex2i(600,1001);
		glVertex2i(800,1001);
		glVertex2i(800,  -1);
	glEnd();
	
	glutSwapBuffers();
	glFlush();
		
}
void idle(){
	draw();
	usleep(1e6/50);
}
void resize(int w, int h){
	glLoadIdentity();
	std::cout<<"Height:"<<h<<" Width:"<<w<<std::endl;
	glViewport(0,0,w,h);
	glTranslatef(-1.0, -1.0, 0.0);
	glScalef(2.0/1000.0, 2.0/1000.0, 0);
	
}
void initShader(){
	
	axesShader = glCreateShader(GL_VERTEX_SHADER);
	
	const std::string shaderfile = "cluster.vert";
	std::string shadersource;
	std::cout<<"Reading shader source from file:"<<shaderfile<<std::endl;
	
	if ( !loadShaderSource(shaderfile, shadersource) )
	{
		std::cout<<"Failed to load shader:"<<shaderfile<<" quiting!"<<std::endl;
		exit(1);
	}

	std::cout<<"Loaded Shader! Its length is:" <<  (int)shadersource.size() <<std::endl;
	GLchar *source = new GLchar[5000];
	memcpy(source, shadersource.c_str(), shadersource.size());
	
 	int len = strlen(source);
//	std::cout<<"SHADER SOURCE:\n"<<source<<std::endl;
	glShaderSource(axesShader, 1, (const GLchar**) &source, (const GLint *) &len);
	checkGlError();
	
	std::cout<<"Compiling shader!"<<std::endl;
	glCompileShader(axesShader);
	checkGlError();
	
	std::cout<<"Creating glProgram"<<std::endl;
	shaderProg = glCreateProgram();
	std::cout<<"\t created with handle:"<<shaderProg<<std::endl;
	checkGlError();
	
	std::cout<<"\tAttaching Shader"<<std::endl;
	glAttachShader(shaderProg, axesShader);
	checkGlError();
	
	std::cout<<"Linking Shader Program"<<std::endl;
	glLinkProgram(shaderProg);
	checkGlError();
	std::cout<<"DONE!"<<std::endl;
	
}
int main(int argc, char* argv[]){
	
	srand ( time(NULL) );
  
	int val=0;
		
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
	
	glewInit();
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
	else {
		printf("Not totally ready   \n");
		exit(1);
	}
	initPoints();
	initShader();
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