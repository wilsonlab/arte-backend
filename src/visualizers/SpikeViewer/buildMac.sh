 g++ -framework GLUT -framework OpenGL -framework Cocoa  -I ../../../include/netcom/ -I ../../../include/common/  -D__STDC_LIMIT_MACROS SpikeViewer.cpp TetrodePlot.cpp PlotUtils.cpp ../../netcom/*.cpp -o SpikeViewer 