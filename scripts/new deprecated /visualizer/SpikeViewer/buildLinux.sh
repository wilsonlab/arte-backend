g++ -lglut -lGL -lGLU -lGLEW  -I ../include/netcom/ -I ../../../include/common/  -D__STDC_LIMIT_MACROS SpikeViewer.cpp TetrodePlot.cpp glutViewer.cpp PlotUtils.cpp ../include/netcom/*.cpp -o SpikeViewer 
