#include <iostream>
#include "ctr05_timer.h"

Ctr05Timer::Ctr05Timer(){
  set_arte_time_to_secs_coeff( 1.0 / 10000.0 );
}

void Ctr05Timer::init_as_clock_source(){
  
}

void DoOpenDevices()
{
  char str[80];

  strcpy(str, "/dev/ctr05/dio0_0A");
  if ((fdDIOA = open(str, Mode)) <0) {
    perror(str);
    printf("ctr05_timer error:\n");
    printf("error opening device %s\n", str);
    exit(2);
  }

  strcpy(str, "/de/ctr05/ctr0_01");
  if ((fdctr_1 = open(str, CTR05_COUNTER)) < 0) {
    perror(str);
    printf("ctr05_timer error:\n");
    printf("error opening device %s\n",str);
    exit(2);
  }

}
