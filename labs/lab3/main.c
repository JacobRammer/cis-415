/*
* Description: <write a brief description of your lab>
*
* Author: Jacob Rammer
*
* Date: 10/7/2020
*
* Notes: 
* 1. I removed all print statements from command.c
* I also just commented stuff out that from lab 2 I don't think 
* I needed instead of just deleting everything
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "command.h"
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
	FILE* fName;
	int fileMode = 0;  // run in filemode?
	// int lineSize;
	lineBuffer = (char *)malloc(bufferSize * sizeof(char));

	// strtok buffer
	// char str[] = "Hi there";

	// Check to see if memory was allocated
	if (lineBuffer == NULL)
		EXIT_FAILURE;

	// Open argv[1], the file name
	char line[256];
	// if(argc > 1)
	// {
	// 	fName = fopen(argv[1], "r");
	// 	fileMode = 1;
	// 	if (fName == NULL)
	// 	{
	// 		printf("No valid file\n");
	// 		exit(EXIT_FAILURE);
	// 	}
	// }
	

	/*main run loop*/
	/* Print >>> then get the input string */
	if(fileMode == 1)
	{
		while (fgets(lineBuffer, bufferSize, fName) != NULL)
		{
			int counter = 0;
			tokbuffer = strtok_r(lineBuffer, " ", &savePtr); //tokenize lineBuffer
			printf("-f option\n");
			while (tokbuffer != NULL && strcmp(tokbuffer, "\n"))
			{
				/* strcmp to break on new lines*/
				printf("T%d: %s\n", counter, tokbuffer);
				tokbuffer = strtok_r(NULL, " ", &savePtr);
				counter++;
			}
		}
	}
	else // standard mode (not file mode)
	{
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
					if(strcmp(tokbuffer, "lfcat\n") != 0)
					{
						printf("Error: Unrecongnized command!\n");
						break;
					}
					lfcat();
					break;
					// /* Tokenize the input string */
					// printf("T%d: %s\n", counter, tokbuffer);
					// tokbuffer = strtok_r(NULL, " \n", &savePtr);
					// counter++;
				}
			}
		}
	}

	/*Free the allocated memory*/
	// if(fileMode == 1)
	// 	fclose(fName);
	free(lineBuffer);

	return 1;
}
/*-----------------------------Program End-----------------------------------*/
