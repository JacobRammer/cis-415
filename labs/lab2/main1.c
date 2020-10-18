/*
* Description: <write a brief description of your lab>
*
* Author: Jacob Rammer
*
* Date: 10/7/2020
*
* Notes: 
* 1. When redirecting output to a file, the format is screwed up
*	 in the terminal. Without the > it works as expected. Not my issue?
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*---------------------------------------------------------------------------*/

/*-----------------------------Program Main----------------------------------*/
int main() {
	/* Main Function Variables */
	
	/* Allocate memory for the input buffer. */
	char* lineBuffer;
	char* savePtr;
	size_t bufferSize = 100;
	size_t characters;
	lineBuffer = (char*)malloc(bufferSize * sizeof(char));

	// strtok buffer
	// char str[] = "Hi there";	

	// Check to see if memory was allocated
	if(lineBuffer == NULL)
		exit(EXIT_FAILURE);
	
	
	/*main run loop*/
		/* Print >>> then get the input string */
	while (1)
	{
		printf(">>> ");
		characters = getline(&lineBuffer, &bufferSize, stdin); // read console
		// printf("You typed: %s\n", lineBuffer);
		char *tokbuffer = strtok_r(lineBuffer, " ", &savePtr);

		/* If the user entered <exit> then exit the loop */
		if (!strcmp(tokbuffer, "exit\n")) // exit completely
			break;

		int counter = 0; // represent token counter

		/* Display each token */
		while (tokbuffer != NULL)
		{
			if (!strcmp(tokbuffer, "\n"))
				// don't print newline on just enter being pressed
				break;
			else
			{
				/* Tokenize the input string */
				printf("T%d: %s\n", counter, tokbuffer);
				tokbuffer = strtok_r(NULL, " \n", &savePtr);
				counter++;
			}
		}

		}
			
	/*Free the allocated memory*/
	free(lineBuffer);

	return 1;
}
/*-----------------------------Program End-----------------------------------*/
