#include "command.h"
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

/*
I wasn't sure if we were allowed to add auxillary functions. 
I'm sure we can but opted into not adding them. The for loop
that removes teh '\n' from strings should have been thrown
into its own function
*/

void listDir()
{
    /* Uses dirent.h and a struct to store file info */

    DIR* currentDirectory;
    char cwd[255]; /* Stores the cwd path: /usr/blabla */
    struct dirent *directory; /* Struct that holds file info */
    

    if(getcwd(cwd, sizeof(cwd)) != NULL) /*  */
    {
        // printf("Current working directory: %s\n", cwd);
        currentDirectory = opendir(cwd);
        while((directory = readdir(currentDirectory)) != NULL)
            {
                write(1, directory->d_name, strlen(directory->d_name));
                write(1, " ", 1);
            }
    }
    else
    {
        char* error = "cwd error";
        write(1, error, strlen(error));
        write(1, "\n", 1);
    }
    
    closedir(currentDirectory);
    write(1, "\n", 1);
    
}

void showCurrentDir()
{
    char cwd[255];
    if(getcwd(cwd, sizeof(cwd)) != NULL)
    {
        write(1, cwd, strlen(cwd));
        write(1, "\n", 1);
    }
}

void displayFile(char* fileName)
{
    int fileNameLength = strlen(fileName) + 1;  // save length for forloop below
    char newFileName[fileNameLength]; 
    ssize_t numCharacters;  // store how many characters read using the read() function
    char buffer[255];
    // buffer = malloc(sizeof(char) * 255);
    // newFileName = malloc(strlen(fileName));

    // Set string to NULL to appease the Valgrind Gods
    memset(newFileName, '\0', sizeof(char) * fileNameLength);
    memset(buffer, '\0', sizeof(char) * 255);

    for (int i = 0; i < fileNameLength; i++)
    {
        /*
        This just removes the "\n" gathered by getline if
        the command is entered as "cat test.txt[\n]"
        */
        if (strcmp(&fileName[i], "\n") != 0)
        {
            // printf("filename[i] %c\n", fileName[i]);
            strncat(newFileName, &fileName[i], 1);
        }
    }
    // printf("New string %s\n", newFileName);
    int catFile = open(newFileName, O_RDONLY); // open in read only

    if(catFile == -1)
    {
        char* errorMessage = "Error opening file: ";
        write(1, errorMessage, strlen(errorMessage));
        write(1, newFileName, strlen(newFileName));
        write(1, "\n", 1);
    }
    else
    {
        numCharacters = read(catFile, buffer, 255);
        while(numCharacters != 0)
        {
            write(1, buffer, numCharacters);
            numCharacters = read(catFile, buffer, 255);
        }
        close(catFile);
    }
    
    // free(buffer);
    // free(newFileName);
    // write(1, "\n", 1);
}

void makeDir(char *dirName)
{
    int length = strlen(dirName);
    char newName[length];
    memset(newName, '\0', sizeof(char) * length);
    for(int i = 0; i < length; i++)
    {
        /*
        Remove the trailing \n if there is one
        */
        if (strcmp(&dirName[i], "\n") != 0)
        {
            // printf("filename[i] %c\n", fileName[i]);
            strncat(newName, &dirName[i], 1);
        }
    }

    mkdir(newName, 0777);  // man page says 0777 is default
}

void deleteFile(char *filename)
{
    int file;
    int length = strlen(filename);
    char newFileName[length];
    memset(newFileName, '\0', sizeof(char) * length);
    char errorMessage[] = "Error removing file\n";
    char successMessage[] = "Removed file: ";

    for(int i = 0; i < length; i++)
    {
        /*
        Remove the trailing \n if there is one
        */
        if(strcmp(&filename[i], "\n") != 0)
        {
            strncat(newFileName, &filename[i], 1);
        }
    }
    if(unlink(newFileName) != 0)
    {
        write(1, errorMessage, strlen(errorMessage));
    }
    else
    {
        write(1, successMessage, strlen(successMessage));
        write(1, newFileName, strlen(newFileName));
        write(1, "\n", 1);
    }  
}

void changeDir(char* dirName)
{
    int length = strlen(dirName);
    char newDirName[length];
    memset(newDirName, '\0', sizeof(char) * length);

    for (int i = 0; i < length; i++)
    {
        /*
        Remove the trailing \n if there is one
        */
        if (strcmp(&dirName[i], "\n") != 0)
        {
            strncat(newDirName, &dirName[i], 1);
        }
    }
    if(chdir(newDirName) != 0)
    {
        char *message = "Error changing directory";
        write(1, message, strlen(message));
        write(1, "\n", 1);
    }
    
}

void moveFile(char *sourcePath, char *destinationPath)  
{
    // int sourceLength = strlen(sourcePath);
    // int destinationLength = strlen(destinationPath);
    // char newSource[sourceLength];
    // char newDestination[destinationLength];

    // memset(newSource, '\0', sizeof(char) * sourceLength);
    // memset(newDestination, '\0', sizeof(char) * destinationLength);

    // char *cwd = malloc(sizeof(char) * 255);
    // getcwd(cwd, 255);

    // if(strcmp(newDestination, ".") == 0)
    //     changeDir(cwd);

    // for (int i = 0; i < sourceLength; i++)
    // {
    //     /*
    //     Remove the trailing \n if there is one
    //     */
    //     if (strcmp(&sourcePath[i], "\n") != 0)
    //     {
    //         strncat(newSource, &sourcePath[i], 1);
    //     }
    // }

    // for (int i = 0; i < destinationLength; i++)
    // {
    //     /*
    //     Remove the trailing \n if there is one
    //     */
    //     if (strcmp(&destinationPath[i], "\n") != 0)
    //     {
    //         strncat(newDestination, &destinationPath[i], 1);
    //     }
    // }
    // printf("New source path: %s\n", newSource);
    // printf("New dest path: %s\n", newDestination);
    // if(rename(newSource, newDestination) == 0)
    // {
    //     printf("Moved file\n");
    // }
    // else
    // {
    //     printf("Move file error\n");
    // }

    copyFile(sourcePath, destinationPath);
    unlink(sourcePath);
    
}

void copyFile(char *sourcePath, char *destinationPath)
{
    int sourceLength = strlen(sourcePath);
    int destinationLength = strlen(destinationPath);
    char newSource[sourceLength];
    char newDestination[destinationLength];
    char finalDest[sourceLength + destinationLength];

    memset(newSource, '\0', sizeof(char) * sourceLength);
    memset(newDestination, '\0', sizeof(char) * destinationLength);
    memset(finalDest, '\0', sizeof(char) * (destinationLength + sourceLength));
    for (int i = 0; i < sourceLength; i++)
    {
        /*
        Remove the trailing \n if there is one
        */
        if (strcmp(&sourcePath[i], "\n") != 0)
        {
            strncat(newSource, &sourcePath[i], 1);
        }
    }

    for (int i = 0; i < destinationLength; i++)
    {
        /*
        Remove the trailing \n if there is one
        */
        if (strcmp(&destinationPath[i], "\n") != 0)
        {
            strncat(newDestination, &destinationPath[i], 1);
        }
    }

    int source = open(newSource, O_RDONLY);  // open read only
    ssize_t characters;
    char* buffer = malloc(sizeof(char) * 255);
    char* cwd = malloc(sizeof(char) * 255);
    getcwd(cwd, 255);
    char* name;
    char* destName;
    char* next;

    /*
    I need to remove the "/" if I'm given a full / absolute path
    Since cp can also rename files when moving them, this is how i'm 
    dealing with it. 
    */
    if (strcmp(newDestination, ".") != 0)
    { 
        /*
        If the destination is pwd, no need to change file name
        */
       
        char *temp = strtok(newDestination, "/");
        while (temp != NULL)
        {
            name = temp;
            temp = strtok(NULL, "/");
            // printf("Yo\n");
        }
    }else
    {
        /*
        If the destination is not the same name as source, 
        strip off everything and update the name;
        */
        char *temp = strtok(newSource, "/");
        while (temp != NULL)
        {
            name = temp;
            temp = strtok(NULL, "/");
            // printf("here\n");
        }
    }
    
//    printf("Name is: %s\n", name);


   chdir(finalDest);
//    printf("Name is: %s\n", name);

//    printf("cwd is %s\n", cwd);
//    if (strcmp(newDestination, ".") == 0)
//        chdir(cwd);

//    printf("New Dest: %s\n", newDestination);
//    printf("Final destination: %s\n", cwd);

   int dest = open(name, O_CREAT); // create the file
   chmod(name, 0770);
   close(dest);
   characters = read(source, buffer, 255);
   dest = open(name, O_WRONLY);
   while (characters > 0)
   {
       write(dest, buffer, characters);
       characters = read(source, buffer, 255);
    }
    
    free(buffer);
    free(cwd);
    close(dest);
    close(source);
    
}
