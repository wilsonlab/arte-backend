#include <getopt.h>
#ifndef VIEWER_COMMAND_DEFS_
#define VIEWER_COMMAND_DEFS_

// ===================================
// Keyboard & Command Function Headers
// ===================================

void initCommandListAndMap();
void keyPressedFn(unsigned char key, int x, int y);
void specialKeyFn(int key, int x, int y);

void enterCommandArg(char key);
void dispCommandString();

bool executeCommand(char * cmd);

void clearWindow();
void resetSeqNum();
void quit();
void scaleUp();
void scaleDown();
void shiftUp();
void shiftDown();
void resetScale();
void nextColor();
void prevColor();
void showHelp();

void setGainAll(void*);
void setGainSingle(void*);
void setTholdAll(void *);
void setTholdSingle(void*);
void setNPostSamps(void *);
void setWindowTimeLen(void*);
void setFrameRate(void*);
void setChannelLabel(void*);

void setPortNumber(void*);
void setWindowXPos(void*);
void setWindowYPos(void*);
void setWindowHeight(void*);
void setWindowWidth(void*);
void setWindowName(void*);
void setChannelLabel(void*);

void setPortNum(char *p);
bool updateWinLen(double l);
void updateFrameRate(int rate);
void setChannelLabel(int n, char* s, int l);

void drawString(float x, float y, char *string);

// ===================================
// 		Command Variables
// ===================================
static int const cmdStrLen = 500;
static char cmd[cmdStrLen];
static int cmdStrIdx = 0;


// Command Engine Map Declaration
typedef void cmdfunc_t(void);
typedef void cmdfunc2_t(void*);
typedef std::map<char, cmdfunc_t*> cmdfnmap_t;
typedef std::map<char, cmdfunc2_t*> cmdfnmap2_t;

static cmdfnmap_t quickCmdMap;
static std::set<int> slowCmdSet;
static cmdfnmap2_t slowCmdMap;

// Command Engine State Variables
static int const CMD_STATE_QUICK = 0; // state where quick commands get executed
static int const CMD_STATE_SLOW = 1;  // state where user specifies what command is to be executed
static int const CMD_STATE_SLOW_ARG = 2;// state where user enters in the command argument
static int CMD_CUR_STATE = CMD_STATE_QUICK;

// Simple commands that respond immediately
static int const CMD_CLEAR_WIN 	= 'c';
static int const CMD_SCALE_UP 	= '=';
static int const CMD_SCALE_DOWN = '-';
static int const CMD_SHIFT_UP 	= '+';
static int const CMD_SHIFT_DOWN = '_';
static int const CMD_RESET_SCALE= 'R';
static int const CMD_PREV_COL 	= '[';
static int const CMD_NEXT_COL 	= ']';
static int const CMD_HELP 		= '?';

// Commands that require CTRL + KEY
static int const CMD_QUIT = 17;
static int const CMD_RESET_SEQ = 18;

// Commands that require an input string
static int const CMD_STR_MAX_LEN 	= 16; 
static int const CMD_GAIN_ALL 		= 'g';
static int const CMD_GAIN_SINGLE 	= 'G';
static int const CMD_THOLD_ALL 		= 'T';
static int const CMD_THOLD_SINGLE 	= 't';
static int const CMD_NULL 			=  0;
static int const CMD_SET_WIN_LEN 	= 'L';
static int const CMD_SET_FRAMERATE 	= 'R';
static int const CMD_SET_POST_SAMPS = 'S';

static int const KEY_DEL = 127;
static int const KEY_BKSP = 8;
static int const KEY_ENTER = 13;

static int currentCommand = 0;
static bool enteringCommand = false;
static int selectedWaveform = 0;

// ===================================
// 		options parsing 
// ===================================

static const char CMD_SET_WIN_NAME 	= 'n';
static const char CMD_SET_WIN_POSX	= 'x';
static const char CMD_SET_WIN_POSY	= 'y';
static const char CMD_SET_WIN_W		= 'w';
static const char CMD_SET_WIN_H		= 'h';
static const char CMD_SET_PORT		= 'p';
static const char CMD_SET_LBL_CH00	= '0';
static const char CMD_SET_LBL_CH01	= '1';
static const char CMD_SET_LBL_CH02	= '2';
static const char CMD_SET_LBL_CH03	= '3';
static const char CMD_SET_LBL_CH04	= '4';
static const char CMD_SET_LBL_CH05	= '5';
static const char CMD_SET_LBL_CH06	= '6';
static const char CMD_SET_LBL_CH07	= '7';

static struct option long_options[] =
{
	{"windowname",required_argument, 0, CMD_SET_WIN_NAME},
	{"xposition", required_argument, 0, CMD_SET_WIN_POSX},
	{"yposition", required_argument, 0, CMD_SET_WIN_POSY},
	{"width",     required_argument, 0, CMD_SET_WIN_W},
	{"height",    required_argument, 0, CMD_SET_WIN_H},
	{"port",	  required_argument, 0, CMD_SET_PORT},
	{"framerate", required_argument, 0, CMD_SET_FRAMERATE},
	{"ch00label", required_argument, 0, CMD_SET_LBL_CH00},
	{"ch01label", required_argument, 0, CMD_SET_LBL_CH01},
	{"ch02label", required_argument, 0, CMD_SET_LBL_CH02},
	{"ch03label", required_argument, 0, CMD_SET_LBL_CH03},
	{"ch04label", required_argument, 0, CMD_SET_LBL_CH04},
	{"ch05label", required_argument, 0, CMD_SET_LBL_CH05},
	{"ch06label", required_argument, 0, CMD_SET_LBL_CH06},
	{"ch07label", required_argument, 0, CMD_SET_LBL_CH07},
	{0, 0, 0, 0}
};

void parseCommandLineArgs(int argc, char**argv);

#endif
