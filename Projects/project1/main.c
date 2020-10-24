#include "command.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *removeCharacter(char *command)
{
    int length = strlen(command) + 1;
    char *retVal = (char *)malloc(sizeof(char) * length);
    memset(retVal, '\0', sizeof(char) * length);
    // printf("I get here\n");
    for (int i = 0; i < length; i++)
    {
        if (command[i] != ' ' && command[i] != '\n')
            strncat(retVal, &command[i], 1);
    }
    // printf("Command is: %s\n", retVal);

    return retVal;
}

int main(int argc, char *argv[])
{
    
    int fileMode = 0;
    int characters;
    char* lineBuffer;  /* stores the buffer of terminal input */
    char* savePtr;  /* stores the remaining of lineBuffer for strtok_r */
    char* tempSavePtr;
    char* tokBuffer;
    size_t bufferSize = 256;
    lineBuffer = (char*)malloc(bufferSize * sizeof(char));
    
    FILE* fName;
    FILE* output;

    if(argc > 1)  /* check to see if we should run in file mode */
    {
        if(strcmp(argv[1], "-f") == 0)
        {
            fileMode = 1;
            output = freopen("output.txt", "w+", stdout);
            // printf("Running in file mode\n");
        }

        /* Check to see if input file is valid */
        fName = fopen(argv[2], "r");
        if(fName == NULL)
        {
            printf("No valid file: %s\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }

    if(fileMode)
    {
        // output = freopen("output.txt", "w+", stdout);
        char* line;
        char* argument;
        char* command;  // stores command after any weird char removed
        char* tempCommand;
        char* commandPtr;
        char* tempArgument1;
        char* tempArgument2;
        int len = 0;
        while(fgets(lineBuffer, bufferSize, fName) != NULL)
        {
            // printf("Line is: %s", lineBuffer);
            line = strtok_r(lineBuffer, ";", &savePtr);  // place before while(saveptr != null)
            // command = malloc(sizeof(char) * 1024);
            while(line)
            {
                tempCommand = strtok_r(line, " ", &commandPtr);  // place after?
                command = removeCharacter(tempCommand);
                if(strcmp(command, "ls") == 0 && strlen(commandPtr) == 0)
                {
                    listDir();
                }
                else if(strcmp(command, "pwd") == 0 && strlen(commandPtr) == 0)
                {
                    showCurrentDir();
                }
                else if(strcmp(command, "mkdir") == 0 & strlen(commandPtr) > 0)
                {
                    tempArgument1 = strtok_r(NULL, " ", &commandPtr);
                    // printf("strlen %ld\n", strlen(commandPtr));
                    if(strlen(commandPtr) == 0)
                    {
                        // printf("hi\n");
                        makeDir(tempArgument1);
                    }
                    else
                    {
                        printf("Error! Incorrect syntax. No control code found\n");
                    }
                    
                    // printf("temp argument 1: %s\n", tempArgument1);
                    // printf("commandptr: %s\n", commandPtr);
                }
                else if (strcmp(command, "cp") == 0 & strlen(commandPtr) > 0)
                {
                    // cp needs 2 args
                    tempArgument1 = strtok_r(NULL, " ", &commandPtr);
                    if (strlen(commandPtr) > 0)
                        tempArgument2 = strtok_r(NULL, " ", &commandPtr);
                    if(strlen(commandPtr) == 0)
                        copyFile(tempArgument1, tempArgument2);
                    else
                    {
                        printf("Error! Incorrect syntax. No control code found\n");
                    }
                    
                }
                else if(strcmp(command, "mv") == 0)
                {
                    tempArgument1 = strtok_r(NULL, " ", &commandPtr);
                    tempArgument2 = strtok_r(NULL, " ", &commandPtr);
                    if(strlen(commandPtr) == 0)
                        moveFile(tempArgument1, tempArgument2);
                    else
                    {
                        printf("Error! Incorrect syntax. No control code found\n");
                    }
                }
                else if(strcmp(command, "rm") == 0)
                {
                    tempArgument1 = strtok_r(NULL, " ", &commandPtr);
                    if(strlen(commandPtr) == 0)
                        deleteFile(tempArgument1);
                    else
                    {
                        printf("Error! Incorrect syntax. No control code found\n");
                    }
                }
                else if(strcmp(command, "cat") == 0)
                {
                    tempArgument1 = strtok_r(NULL, " ", &commandPtr);
                    if(strlen(commandPtr) == 0)
                        displayFile(tempArgument1);
                    else
                    {
                        printf("Error! Incorrect syntax. No control code found\n");
                    }
                }
                else if(strcmp(command, "cd") == 0)
                {
                    tempArgument1 = strtok_r(NULL, " ", &commandPtr);
                    if(strlen(commandPtr) == 0)
                        changeDir(tempArgument1);
                    else
                    {
                        printf("Error! Incorrect syntax. No control code found\n");
                    }
                }
                else
                {
                    printf("Error! Incorrect syntax. No control code found\n");
                }
                
                free(command);
                line = strtok_r(NULL, ";", &savePtr);
            }
        }

        
    }else
    {
        while (1)
        {
            printf(">>> ");
            characters = getline(&lineBuffer, &bufferSize, stdin);

            char *line;
            char *argument;
            char *command; // stores command after any weird char removed
            char *tempCommand;
            char *commandPtr;
            char *tempArgument1;
            char *tempArgument2;
            int len = 0;
            line = strtok_r(lineBuffer, ";", &savePtr); // place before while(saveptr != null)
            // command = malloc(sizeof(char) * 1024);
            while (line)
            {
                tempCommand = strtok_r(line, " ", &commandPtr); // place after?
                command = removeCharacter(tempCommand);
                if (strcmp(command, "ls") == 0 && strlen(commandPtr) == 0)
                {
                    listDir();
                }
                else if (strcmp(command, "pwd") == 0 && strlen(commandPtr) == 0)
                {
                    showCurrentDir();
                }
                else if (strcmp(command, "mkdir") == 0)
                {
                    if(strlen(commandPtr) > 0)
                        {
                            tempArgument1 = strtok_r(NULL, " ", &commandPtr);
                            makeDir(tempArgument1);
                        }
                        
                    // printf("strlen %ld\n", strlen(commandPtr));
                    // if (strlen(commandPtr) == 0)
                    // {
                    //     // printf("hi\n");
                    //     makeDir(tempArgument1);
                    // }
                    else
                    {
                        printf("Error! Incorrect syntax. No control code found\n");
                    }

                    // printf("temp argument 1: %s\n", tempArgument1);
                    // printf("commandptr: %s\n", commandPtr);
                }
                else if (strcmp(command, "cp") == 0)
                {
                    // cp needs 2 args
                    tempArgument1 = strtok_r(NULL, " ", &commandPtr);
                    tempArgument2 = strtok_r(NULL, " ", &commandPtr);
                    // printf("Commandptr is %s %ld\n", commandPtr, strlen(commandPtr));
                    if (strlen(commandPtr) == 0)
                        copyFile(tempArgument1, tempArgument2);
                    else
                    {
                        printf("Error! Incorrect syntax. No control code found\n");
                    }
                }
                else if (strcmp(command, "mv") == 0)
                {
                    tempArgument1 = strtok_r(NULL, " ", &commandPtr);
                    tempArgument2 = strtok_r(NULL, " ", &commandPtr);
                    if (strlen(commandPtr) == 0)
                        moveFile(tempArgument1, tempArgument2);
                    else
                    {
                        printf("Error! Incorrect syntax. No control code found\n");
                    }
                }
                else if (strcmp(command, "rm") == 0)
                {
                    tempArgument1 = strtok_r(NULL, " ", &commandPtr);
                    if (strlen(commandPtr) == 0)
                        deleteFile(tempArgument1);
                    else
                    {
                        printf("Error! Incorrect syntax. No control code found\n");
                    }
                }
                else if (strcmp(command, "cat") == 0)
                {
                    tempArgument1 = strtok_r(NULL, " ", &commandPtr);
                    if (strlen(commandPtr) == 0)
                        displayFile(tempArgument1);
                    else
                    {
                        printf("Error! Incorrect syntax. No control code found\n");
                    }
                }
                else if (strcmp(command, "cd") == 0)
                {
                    tempArgument1 = strtok_r(NULL, " ", &commandPtr);
                    if (strlen(commandPtr) == 0)
                        changeDir(tempArgument1);
                    else
                    {
                        printf("Error! Incorrect syntax. No control code found\n");
                    }
                }
                else if(strcmp(command, "exit") == 0)
                    exit(EXIT_SUCCESS);
                else
                {
                    printf("Error! Incorrect syntax. No control code found\n");
                }

                free(command);
                line = strtok_r(NULL, ";", &savePtr);
            }
        }
    }
    
    

    

   if(fileMode)
   {
       fclose(fName);
    //    fclose(output);
   }
    free(lineBuffer);
    // fclose(output);
    return 0;
}