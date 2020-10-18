/*
* Description: <write a brief description of your lab>
*
* Author: Jacob Rammer
*
* Date: 10/7/2020
*
* Notes: 
* 1. I think I did this the way we were supposed to.
*	 I tried using getline to read every line in the file
* 	 one-by-one but couldn't get it to work. So I settled
* 	 for this
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*---------------------------------------------------------------------------*/

/*-----------------------------Program Main----------------------------------*/
int main(int argc, char *argv[])
{
	/* Main Function Variables */

	/* Allocate memory for the input buffer. */
	char *lineBuffer;  // buffer for each line in file
	char *tokbuffer;  // buffer for tokenized lineBuffer
	char* savePtr;  // save the rest of the string for strtok_r
	size_t bufferSize = 256;
	size_t characters;
	// int lineSize;
	lineBuffer = (char *)malloc(bufferSize * sizeof(char));

	// strtok buffer
	// char str[] = "Hi there";

	// Check to see if memory was allocated
	if (lineBuffer == NULL)
		EXIT_FAILURE;

	// Open argv[1], the file name
	char line[256];
	FILE *fName = fopen(argv[1], "r");
	if (fName == NULL)
	{
		printf("No valid file\n");
		exit(EXIT_FAILURE);
	}

	/*main run loop*/
	/* Print >>> then get the input string */
	while (fgets(lineBuffer, bufferSize, fName) != NULL)
	{
		int counter = 0;
		tokbuffer = strtok_r(lineBuffer, " ", &savePtr);  //tokenize lineBuffer
		while(tokbuffer != NULL && strcmp(tokbuffer, "\n"))
		{
			/* strcmp to break on new lines*/
			printf("T%d: %s\n", counter, tokbuffer);
			tokbuffer = strtok_r(NULL, " ", &savePtr);
			counter++;
		}
	}

	/*Free the allocated memory*/
	fclose(fName);
	free(lineBuffer);

	return 1;
}
/*-----------------------------Program End-----------------------------------*/
