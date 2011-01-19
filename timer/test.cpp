#include "timer.h"
#include <iostream>
#include <stdio.h>

// <float *> parse_line_for_nums(string, 5, float *);

int main(int argc, char *argv[]){

	double rate = 10e6;
	Timer myTime(rate, 1000); //	 = new Timer(rate);
	myTime.startTimer();
	double ts = 0;

	std::cout << "Running, Ctrl-C to quit \n";
	std::cout <<"";

	for (int n = 0; n <= 1000000; n++){
		printf("\rTick %f",myTime.tick());
		fflush(stdout);
	}
	std::cout<<"\n";

	return 0;
}
