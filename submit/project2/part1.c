#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int commands(FILE* file)
{
    /*
    Counts the number of commands in a file. 
    A command occupies one line of a test file. 
    So this function basically counts the number of
    lines in a file
    */

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
    /*
    Counts the nunber of arguments per line. An argument
    defined here is a command seperated by a space
    */

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

void removeNewline(char* input)
{
    int length = strlen(input);
    // printf("Input is %s\n", input);
    for(int i = 0; i < length; i++)
    {
        char temp = input[i];
        if(strcmp(&input[i], "\n") == 0)
        {
            input[i] = '\0';
        }
    }

    // printf("Return string: %s\n", input);
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

    size_t bufferSize = 256;
    char *lineBuffer;  // hold line with newline
    lineBuffer = (char *)malloc(bufferSize * sizeof(char));
    
    ssize_t characters = 0;
    fName = fopen(argv[1], "r");
    characters = getline(&lineBuffer, &bufferSize, fName);
    removeNewline(lineBuffer);
    pid_t childArray[numCommands];
    char* savePtr;
    int currentChild = 0;  // Saves which command we're on

    while(characters > 0)
    {
        // printf("LineBuffer: %s\n", lineBuffer);
        int numArgs = arguments(lineBuffer);
        char* argumentArray[numArgs + 1];  // for null
        argumentArray[numArgs] = NULL;
        char* tempCommand = strtok_r(lineBuffer, " ", &savePtr);
        for(int i = 0; i < numArgs; i++)
        {
            /*
            create a command array for execvp. For example, 
            break ls -a -r -s up into arry[0] = ls, arr[1] = -a etc,
            */

            argumentArray[i] = tempCommand;
            // printf("Test: %s\n", argumentArray[i]);
            tempCommand = strtok_r(NULL, " ", &savePtr);
        }

        // After the command array has been created, start the fork
        childArray[currentChild] = fork();
        if(childArray[currentChild] == 0)
        {
            if(childArray[currentChild] = execvp(argumentArray[0], argumentArray) < 0);
                {
                    perror("Execvp error: ");
                    free(lineBuffer);
                    // free(childArray);
                    fclose(fName);
                    exit(-1);
                }
        }

        // Move onto the next command (line in file)
        characters = getline(&lineBuffer, &bufferSize, fName);
        removeNewline(lineBuffer);
        currentChild++;
    }

    int status;
    for(int i = 0; i < numCommands; i++)
        while(waitpid(childArray[i], &status, 0) > 0);
    free(lineBuffer);
    fclose(fName);
    exit(0);
}