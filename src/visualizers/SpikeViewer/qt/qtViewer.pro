TEMPLATE = app
TARGET = glQt
QT += opengl

HEADERS += \
	SpikeViewer.h \
	SpikeViewerWidget.h \
	TetrodePlot.h \
	PlotUtils.h \
	commandDefs.h \
	../../../../include/netcom/netcom.h \
	../../../../include/netcom/datapacket.h

SOURCES += \
    SpikeViewerWidget.cpp \
	qtViewer.cpp \
	SpikeViewer.cpp  \
	TetrodePlot.cpp \
	PlotUtils.cpp \
	../../../../src/netcom/netcom.cpp \
	../../../../src/netcom/datapacket.cpp 
