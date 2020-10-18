#include "command.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    
    int fileMode = 0;
    int characters;
    char* lineBuffer;  /* stores the buffer of terminal input */
    char* savePtr;  /* stores the remaining of lineBuffer for strtok_r */
    char* tokBuffer;
    size_t bufferSize = 100;
    lineBuffer = (char*)malloc(bufferSize * sizeof(char));
    
    FILE* fName;

    if(argc > 1)  /* check to see if we should run in file mode */
    {
        if(strcmp(argv[1], "-f") == 0)
        {
            fileMode = 1;
            printf("Running in file mode\n");
        }

        /* Check to see if input file is valid */
        fName = fopen(argv[2], "r");
        if(fName == NULL)
        {
            printf("No valid file: %s\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }

    while(1)
    {
        printf(">>> ");
        characters = getline(&lineBuffer, &bufferSize, stdin);
        tokBuffer = strtok_r(lineBuffer, " \n", &savePtr);
        printf("token buffer %s\n", tokBuffer);
        if(strcmp(tokBuffer, "ls") == 0)
        {
            listDir();
        }
        else if (strcmp(tokBuffer, "pwd") == 0)
        {
            showCurrentDir();
        }
        else if(strcmp(tokBuffer, "cat") == 0)
        {
            tokBuffer = strtok_r(NULL, " ", &savePtr);
            displayFile(tokBuffer);
        }
        else if(strcmp(tokBuffer, "mkdir") == 0)
            {
                tokBuffer = strtok_r(NULL, " ", &savePtr);
                makeDir(tokBuffer);
            }
            
        else if(strcmp(tokBuffer, "exit") == 0)
            break;

    }

    // fclose(fName);  TODO fix this
    free(lineBuffer);
    return 0;
}