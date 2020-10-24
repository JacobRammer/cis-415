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
                // printf("Line is: %s\n", line);
                // printf("Saveptr is %s\n", savePtr);
                // printf("Command is: %s %ld\n", command, strlen(tempCommand));
                // printf("Command2 is: %s %ld\n", command, strlen(command));
                // printf("cmdptr: %s\n", commandPtr);
                // printf("Saveptr is %s\n", savePtr);
                if(strcmp(command, "ls") == 0 && strlen(commandPtr) == 0)
                {
                    listDir();
                }
                else if(strcmp(command, "pwd") == 0 && strlen(commandPtr) == 0)
                {
                    showCurrentDir();
                }
                else if(strcmp(command, "mkdir") == 0)
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
                else if(strcmp(command, "cp") == 0)
                {
                    // cp needs 2 args
                    tempArgument1 = strtok_r(NULL, " ", &commandPtr);
                    tempArgument2 = strtok_r(NULL, " ", &commandPtr);
                    // printf("Commandptr is %s %ld\n", commandPtr, strlen(commandPtr));
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
        // printf("\n");
        // ret = strrchr(lineBuffer, ';');
        // line = strtok_r(lineBuffer, ";", &savePtr);
        // command = strtok_r(line, " ", &commandPtr);

        
        // printf("Line is: %s\n", line);
        // printf("Command is: %s\n", command);
        // printf("Argumenmt is: %s\n", argument);
        
        //important

        // printf("SavePtr is %s\n", savePtr);
        // printf("commandPtr is: %s\n", commandPtr);
        // char* temp1;
        // temp1 = strtok_r(commandPtr, " ", &commandPtr);
        // printf("temp1 is:%s\n", temp1);
        // printf("temp1 is:%s\n", temp1);
        // char *temp2 = strtok_r(commandPtr, " ", &commandPtr);
        // printf("temp2 is:%s\n", temp2);
        // printf("saveptr is %ld\n", strlen(savePtr));
        // printf("commandPtr is: %s\n", commandPtr);
        // len = strlen(argument);

        //end important

        // command = strtok_r(NULL, " ", &savePtr);
        // argument = strtok_r(NULL, ";", &savePtr);
        // printf("Command is %s\n", command);
        // printf("Ret is: %s\n", argument);
        
        // argument = strtok_r(NULL, ";", &savePtr);
        // printf("new Ret is: %s\n", argument);
        
        /*
        After we get the first command from the line, I then
        need to look at the commands, and do the error checking there. 
        */

        // char *test = strtok_r(ret, " ", &tempSavePtr);
        // printf("test is: %s\n", test);
        // ret = strtok_r(NULL, ";", &savePtr);
        // printf("Ret is: %s\n", ret);
        
    }else
    {
        while (1)
        {
            printf(">>> ");
            characters = getline(&lineBuffer, &bufferSize, stdin);
            tokBuffer = strtok_r(lineBuffer, " \n", &savePtr);
            printf("token buffer %s\n", tokBuffer);
            if (strcmp(tokBuffer, "ls") == 0)
            {
                listDir();
            }
            else if (strcmp(tokBuffer, "pwd") == 0)
            {
                showCurrentDir();
            }
            else if (strcmp(tokBuffer, "cat") == 0)
            {
                tokBuffer = strtok_r(NULL, " ", &savePtr);
                displayFile(tokBuffer);
            }
            else if (strcmp(tokBuffer, "mkdir") == 0)
            {
                tokBuffer = strtok_r(NULL, " ", &savePtr);
                makeDir(tokBuffer);
            }
            else if (strcmp(tokBuffer, "rm") == 0)
            {
                tokBuffer = strtok_r(NULL, " ", &savePtr); // get rid of "rm"
                deleteFile(tokBuffer);
            }
            else if (strcmp(tokBuffer, "cd") == 0)
            {
                tokBuffer = strtok_r(NULL, " ", &savePtr);
                changeDir(tokBuffer);
            }
            else if (strcmp(tokBuffer, "mv") == 0)
            {
                char *source = strtok_r(NULL, " ", &savePtr);
                char *dest = tokBuffer = strtok_r(NULL, " ", &savePtr);
                // printf("mv token is: %s\n", source);
                // printf("mv token is: %s\n", dest);
                moveFile(source, dest);
            }
            else if (strcmp(tokBuffer, "cp") == 0)
            {
                char *source = strtok_r(NULL, " ", &savePtr);
                char *dest = tokBuffer = strtok_r(NULL, " ", &savePtr);
                // printf("mv token is: %s\n", source);
                // printf("mv token is: %s\n", dest);
                copyFile(source, dest);
            }
            else if (strcmp(tokBuffer, "exit") == 0)
                break;
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