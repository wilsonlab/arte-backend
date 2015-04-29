#if defined(__linux__)
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <getopt.h>
#include "TetrodePlot.h"
#include "PlotUtils.h"
#include "commandDefs.h"

	// ===================================
	// 		ports
	// ===================================

	static int startingport;
	//static char portchar[6];
static char *portarray[32]; //took away *

class SpikeViewer{
public:
	SpikeViewer(int, int, int, int, char*[]);
	~SpikeViewer();
	void drawPlot();
	void resizePlot(int w, int h);
	void initPlots();
	void loadAndCompileShader();

	// ===================================
	// 		Command Variables
	// ===================================


	// Command Functions
	void scaleUpAll();
	void scaleUpSel();
	void scaleDownAll();
	void scaleDownSel();
	void shiftUpAll();
	void shiftUpSel();
	void shiftDownAll();
	void shiftDownSel();
	void toggleOverlayAll();
	void toggleOverlaySel();
	void clearAll();
	void clearSel();
	void showHelp();
	void quit();
	
	void keyPressedFn(unsigned char key);
	void specialKeyFn(int key);
	void mouseClickFn(int button, int state, int x, int y);
	
private:
	static const int IDLE_SLEEP_USEC = (1e6)/80;
	int winWidth;
	int winHeight;

	int nCol;
	int nRow;
	
	void *font;
//	const static char app_name[] = "Arte Network Spike Viewer v0.9 - (C) 2011 Stuart Layton, MIT";

	static const int MAX_N_PLOT = 64;

	TetrodePlot *plots[MAX_N_PLOT];

	int nPlots;
	int selectedPlot;

	bool allOverlay;

	// Application Title
	void drawAppTitle();
	void setViewportForTitle();
	void initCommandSets();
	void executeQuickCommand(unsigned char key);

	// Command Engine Map Declaration
	void drawCommandString();
	void setViewportForCommandWin();

	static int const cmdStrLen = 500;
	char cmd[cmdStrLen];
	int cmdStrIdx;
	int cmdWinHeight;
	int cmdWinWidth;
	float cmdWinCol[3];

	std::set<int> quickCmdSet;
	int cmdState;	
	
	bool loadShaderSource(const std::string& filename, std::string& out);

	GLuint shaderProg;

 	int currentCommand;
	bool enteringCommand;

	// ===================================
	// 		options parsing 
	// ===================================

	static const char CMD_SET_WIN_NAME 	= 'n';
	static const char CMD_SET_WIN_POSX	= 'x';
	static const char CMD_SET_WIN_POSY	= 'y';
	static const char CMD_SET_WIN_W		= 'w';
	static const char CMD_SET_WIN_H		= 'h';
	


};
void parseCommandLineArgs(int argc, char**argv);

