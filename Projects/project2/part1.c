#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int commands(FILE* file)
{
    int returnVal = 0;
    int bufferSize = 256;
    char* lineBuffer;
    lineBuffer = (char*)malloc(bufferSize * sizeof(char));
    while(fgets(lineBuffer, bufferSize, file) != NULL)    
    {
        returnVal++;
    }
    
    free(lineBuffer);
    fclose(file);
    return returnVal;
}

int main(int argc, char *argv[])
{
    FILE* fName;
    int numCommands;  // how many commands ie # of lines in file

    fName = fopen(argv[1], "r");
    if(fName == NULL)
    {
        printf("No valid file: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // Get number of commands
    numCommands = commands(fName);
    printf("Num commands is: %d\n", numCommands);
    // fclose(fName);
    // fName = fopen(argv[1], "r");

    int bufferSize = 256;
    char *lineBuffer;
    lineBuffer = (char *)malloc(bufferSize * sizeof(char));
    size_t characters;
    characters = getline(&lineBuffer, &bufferSize, fName);
    while(characters > 0)
    {
        printf("%s", lineBuffer);
    }
    return 0;
}