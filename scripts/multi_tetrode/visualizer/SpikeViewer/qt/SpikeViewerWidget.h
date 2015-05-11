#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <cstdio>
#include <QtOpenGL/QGLWidget>
#include <QTimer>
#include "SpikeViewer.h"

class SpikeViewerWidget : public QGLWidget {

    Q_OBJECT // must include this if you use Qt signals/slots

public:
    SpikeViewerWidget(QWidget *parent = NULL);
	SpikeViewer *s;

protected:
	bool firstDraw;
	
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

	protected slots:
		void animate();
	private:
	 	QTimer animationTimer;
	
};

#endif  /* _GLWIDGET_H */
