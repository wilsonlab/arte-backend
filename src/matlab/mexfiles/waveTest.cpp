/* $Revision: 1.1.6.3 $ */
/*
 *	engdemo.cpp
 *
 *	This is a simple program that illustrates how to call the MATLAB
 *	Engine functions from a C++ program.
 *
 * Copyright 1984-2007 The MathWorks, Inc.
 * All rights reserved
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#define  BUFSIZE 2048

#define nChan 4
#define nSamp 32

int main(){
	Engine *ep;
	mxArray *T = NULL, *result = NULL;
	char buffer[BUFSIZE+1];
    double ts[nSamp];
    
    for (int i=0; i<nSamp; i++)
        ts[i] = i;
    
    //double wave[nChan][nSamp];
	//for (int i=0; i<nChan; i++){
    //    for (int j=0; j<nSamp; j++){
    //        wave[i][j] = j + (32*i);
    //    }
    //}

	/*
	 * Start the MATLAB engine locally by executing the string
	 * "matlab"
	 *
	 * To start the session on a remote host, use the name of
	 * the host as the string rather than \0
	 *
	 * For more complicated cases, use any string with whitespace,
	 * and that string will be executed literally to start MATLAB
	 */
	if (!(ep = engOpen("\0"))) {
		fprintf(stderr, "\nCan't start MATLAB engine\n");
		return EXIT_FAILURE;
	}

	/*
	 * PART I
	 *
	 * For the first half of this demonstration, we will send data
	 * to MATLAB, analyze the data, and plot the result.
	 */

	/* 
	 * Create a variable for our data
	 */
	T = mxCreateDoubleMatrix(1, nSamp, mxREAL);
	memcpy((void *)mxGetPr(T), (void *)ts, sizeof(ts));
	/*
	 * Place the variable T into the MATLAB workspace
	 */
	engPutVariable(ep, "T", T);

	/*
	 * Evaluate a function of time, distance = (1/2)g.*t.^2
	 * (g is the acceleration due to gravity)
	 */
	engEvalString(ep, "D(:,1) = sin(T/5)");

	/*
	 * Plot the result
	 */
	engEvalString(ep, "plot(T,D, '.');");
	engEvalString(ep, "title('Position vs. Time for a falling object');");
	engEvalString(ep, "xlabel('Time (seconds)');");
	engEvalString(ep, "ylabel('Position (meters)');");

	/*
	 * use fgetc() to make sure that we pause long enough to be
	 * able to see the plot
	 */
	printf("Hit return to continue\n\n");
	fgetc(stdin);
	/*
	 * We're done for Part I! Free memory, close MATLAB figure.
	 */
    
	printf("Done for Part I.\n");
	mxDestroyArray(T);
	engEvalString(ep, "close;");

	/*
	 * We're done! Free memory, close MATLAB engine and exit.
	 */
	printf("Done!\n");
	mxDestroyArray(result);
	engClose(ep);
	
	return EXIT_SUCCESS;
}








