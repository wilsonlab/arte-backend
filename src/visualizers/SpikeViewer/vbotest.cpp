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


GLuint VertexVBOID;
GLuint IndexVBOID;
GLuint axesShader;
GLuint shaderProg;

GLuint geometry_array = 0;
GLuint indice_array = 0;

GLfloat *geometry;
GLuint *indices;



void checkGlError(){
	GLenum errCode;
	const GLubyte *errString;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
	    errString = gluErrorString(errCode);
	   fprintf (stderr, "OpenGL Error: %s\n", errString);
	}
//	else
//		std::cout<<"Okay!"<<std::endl;
}
struct SpikeVertex{
	GLint x, y, z, w;
};

SpikeVertex pvertex[MAX_N_POINT];

void initVBO(){
	// Initialize
	

	SpikeVertex baseData[MAX_N_POINT];
	GLuint indices[MAX_N_POINT];

	std::cout<<"Created starting data structures, they are:"<< sizeof(SpikeVertex) * MAX_N_POINT / 1024 <<" Kilobytes"<<std::endl;
	std::cout<<std::flush;
	
	for (int i=0; i<MAX_N_POINT; i++){
		baseData[i].x = rand()%200;
		baseData[i].y = rand()%200+200;
		baseData[i].z = rand()%200+400;
		baseData[i].w = rand()%200+600;
		indices[i] = i;
	}
	/* Fill geometry: 0, 1, 2 = vertex_xyz 
	 *                3, 4, 5 = normal_xyz
	 *                6, 7 = tex_coord_uv
	 */

	glGenBuffers(1, &geometry_array);
	glBindBuffer(GL_ARRAY_BUFFER, geometry_array);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SpikeVertex)*MAX_N_POINT, baseData, GL_DYNAMIC_DRAW);
	// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*MAX_N_POINT, baseData);

	glGenBuffers(1, &indice_array);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indice_array);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*MAX_N_POINT, indices, GL_STATIC_DRAW);
//	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint)*MAX_N_POINT, indices);

}
void drawVBO(){
//	std::cout<<"Drawing VBO!"<<std::endl;
	//Render
	// Step 1
	glBindBuffer(GL_ARRAY_BUFFER, geometry_array);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indice_array);

	// Step 2
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	// Step 3
//	glTexCoordPointer(3, GL_FLOAT, sizeof(GLfloat)*8, (float*)(sizeof(GLfloat)*5));
//	glNormalPointer(GL_FLOAT, sizeof(GLfloat)*8, (float*)(sizeof(GLfloat)*3));
	glVertexPointer(4, GL_INT, sizeof(SpikeVertex), NULL);

	// Step 4
	glDrawElements(GL_POINTS, MAX_N_POINT, GL_UNSIGNED_INT, NULL);

	// Step 5
	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
void destroyVBO(){
	
}
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


void drawPoints(){



	//New shader code here
	for (int i=0; i<6; i++){
		setColor(i);
		glUseProgram(shaderProg);
		int plotAxes = i;
		GLint axes = glGetUniformLocation(shaderProg, "axes"); 
	  	glUniform1i(axes, plotAxes);
	
		drawVBO();
		glUseProgram(0);
	}
	
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
	checkGlError();
	
	std::cout<<"Attaching Shader"<<std::endl;
	glAttachShader(shaderProg, axesShader);
	checkGlError();
	
	std::cout<<"Linking Shader Program"<<std::endl;
	glLinkProgram(shaderProg);
	checkGlError();
	std::cout<<"DONE!\n"<<std::endl;
	
}
int main(int argc, char* argv[]){
	
	srand ( time(NULL) );
  
	int val=0;
		
	glutInit(&argc,argv);
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowPosition( 5, 20);
	glutInitWindowSize(300, 300);
	glutCreateWindow("OpenGL VBO Test using Swizzling 4D Points");

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
	initVBO();
	initShader();
	glutMainLoop( );

	return(0);
}