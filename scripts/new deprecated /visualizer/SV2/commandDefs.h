#ifndef COMMAND_DEFS_H
#define COMMAND_DEFS_H

	// Command Engine State Variables
	static int const CMD_STATE_QUICK = 0; // state where quick commands get executed
	static int const CMD_STATE_SLOW = 1;  // state where user specifies what command is to be executed
	static int const CMD_STATE_SLOW_ARG = 2;// state where user enters in the command argument

	// Simple commands that respond immediately
	static int const CMD_CLEAR_ALL 	= 'C';
	static int const CMD_CLEAR_SEL  = 'c';
	static int const CMD_SCALE_UP_SEL 	= '=';
	static int const CMD_SCALE_DOWN_SEL = '-';
	static int const CMD_SCALE_UP_ALL 	= '+';
	static int const CMD_SCALE_DOWN_ALL = '_';
	static int const CMD_RESET_SCALE    = 'R';
	static int const CMD_SHIFT_UP_SEL 	= ']';
	static int const CMD_SHIFT_DOWN_SEL = '[';
	static int const CMD_SHIFT_UP_ALL 	= '}';
	static int const CMD_SHIFT_DOWN_ALL = '{';
	static int const CMD_OVERLAY_SEL 	= 'o';
	static int const CMD_OVERLAY_ALL	= 'O';

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

#endif