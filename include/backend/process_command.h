#include "arteopt.h"
#include "datapacket.h"
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
#include "timer.h"

#define MAX_N_TOKENS 80
#define MAX_TOKEN_SIZE 80

extern Trode    *trode_array;
extern Lfp_bank *lfp_bank_array;
extern int n_trodes;
extern int n_lfp_banks;
extern Timer arte_timer;

void process_command(command_t *cmd);
void parse_command(char *cmd_str, int *c_argc, char c_argv[MAX_N_TOKENS][MAX_TOKEN_SIZE]);
void num_list_from_c_argv(int c_argc, char c_argv[MAX_N_TOKENS][MAX_TOKEN_SIZE], 
			  int start_ind, int stop_ind, int* num_list);
void build_num_lists(int c_argc, char c_argv[MAX_N_TOKENS][MAX_TOKEN_SIZE],
		     int trode_names[MAX_N_TOKENS], int *n_trode_names,
		     int lfp_bank_names[MAX_N_TOKENS], int *n_lfp_bank_names,
		     int chan_inds[MAX_N_TOKENS], int *n_chan_inds);
void set_threshold(int *trodenames, int n_trodes, int *channames, int n_chans, rdata_t new_threshold);
void set_postsamps(int *trodenames, int n_trodes, int new_postsamps);

int ind_from_trode_name(uint16_t name);
int ind_from_lfp_bank_name(uint16_t name);
