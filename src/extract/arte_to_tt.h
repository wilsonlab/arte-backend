#ifndef ARTE_TO_TT_H_
#define ARTE_TO_TT_H_

#include <stdio.h>
#include <stdint.h>
#include <cstdlib> // apparently needed for exit(0) in Ubuntu
#include "../netcom/datapacket.h"
#include "../global_defs.h"

void write_spike(spike_net_t *spike);
void init_filenames(int argc, char *argv[], char input_filename[], char output_filename[], int *trodename);
void write_file_header(spike_net_t *spike, int argc, char *argv[]);
bool get_next_spike(spike_net_t *spike, int trodename);
//void tfprintf(FILE *fp, char format_str[], ...);
//void ttfprintf(FILE *fp, char format_str[], ...);
void ps();

#endif
