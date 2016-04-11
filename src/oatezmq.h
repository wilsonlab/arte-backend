#ifndef OATEZMQ_H_
#define OATEZMQ_H_

#include <iostream>
#include <fstream>
#include <ctime>
#include <zmq.hpp>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "timer.h"
#include "util.h"
#include <time.h>
#include "arteopt.h"
#include "neural_daq.h"

extern pthread_mutex_t main_file_mutex;
extern Timer arte_timer;
#define EVERY_N_TS_BUFFSIZE 10
extern uint32_t   ts_buffer[EVERY_N_TS_BUFFSIZE];
extern int        ts_buff_cursor;

void *oate(void*);

#endif
