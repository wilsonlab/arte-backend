#ifndef ARTE_TO_MWL_H_
#define ARTE_TO_MWL_H_

#include <stdio.h>
#include <stdint.h>
#include <cstdlib> // apparently needed for exit(0) in Ubuntu
#include "netcom/datapacket.h"
#include "global_defs.h"

void write_mwl(void *arte_packet, packetType_t sourcetype);
void init_filenames(int argc, char *argv[], char input_filename[], char output_filename[], int *sourcename, packetType_t *sourcetype);
void write_file_header(void *arte_packet, int argc, char *argv[], packetType_t sourcetype);
bool get_next_packet(void *arte_packet, int sourcename, packetType_t sourcetype);
void ps();
void interactive_wait(char *wait_str);
void print_packet(void *arte_packet, packetType_t sourcetype);

#endif
