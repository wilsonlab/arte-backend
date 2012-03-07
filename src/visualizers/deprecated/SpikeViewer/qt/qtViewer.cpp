#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include "SpikeViewerWidget.h"
#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    SpikeViewerWidget window;
    window.resize(800,600);
    window.show();

    return app.exec();
}
