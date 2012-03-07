#include <QtGui/QMouseEvent>
#include "SpikeViewerWidget.h"

SpikeViewerWidget::SpikeViewerWidget(QWidget *parent) : QGLWidget(parent) {
    setMouseTracking(true);
	s = new SpikeViewer(4, 4, 800, 600, NULL);
	s->initPlots();
	firstDraw = true;

	animationTimer.setSingleShot(false);
    connect(&animationTimer, SIGNAL(timeout()), this, SLOT(animate()));
    animationTimer.start(1e3/60);
}

void SpikeViewerWidget::initializeGL() {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glEnable(GL_POLYGON_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 0);
}

void SpikeViewerWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
//
//    gluOrtho2D(0, w, 0, h); // set origin to bottom left corner
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT);

	s->resizePlot(w,h);
}

void SpikeViewerWidget::paintGL() {
//	std::cout<<"SpikeViewerWidget::paintGL()"<<std::endl;
   
	if (firstDraw){
		glClear(GL_COLOR_BUFFER_BIT);
		firstDraw = false;
	}
	s->drawPlot();
}



void SpikeViewerWidget::mousePressEvent(QMouseEvent *event) {

}
void SpikeViewerWidget::mouseMoveEvent(QMouseEvent *event) {
    printf("%d, %d\n", event->x(), event->y());
}

void SpikeViewerWidget::keyPressEvent(QKeyEvent* event) {
	std::cout<< (char) event->key() <<" key "<< event->key()<< " val"<<std::endl;
    switch(event->key()) {
    case Qt::Key_Escape:
        close();
        break;
    default:
		s->keyPressedFn(event->key());
        event->ignore();
        break;
    }
}

void SpikeViewerWidget::animate(){
//	std::cout<<"SpikeViewerWidget::animating!"<<std::endl;
	updateGL();
}