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

int arguments(char* line)
{
    int args = 0;
    int length = strlen(line);
    for(int i = 0; i < length; i++)
    {
        char temp = line[i];
        if(line[i] == ' ')
            args++;
    }
    return (args + 1);
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
    // printf("Num commands is: %d\n", numCommands);
    // fclose(fName);
    // fName = fopen(argv[1], "r");

    size_t bufferSize = 256;
    char *lineBuffer;
    lineBuffer = (char *)malloc(bufferSize * sizeof(char));
    ssize_t characters = 0;
    fName = fopen(argv[1], "r");
    characters = getline(&lineBuffer, &bufferSize, fName);
    while(characters > 0)
    {
        // printf("%s", lineBuffer);
        int temp = arguments(lineBuffer);
        printf("Args: %d\n", temp);
        characters = getline(&lineBuffer, &bufferSize, fName);
    }

    free(lineBuffer);
    fclose(fName);
    return 0;
}