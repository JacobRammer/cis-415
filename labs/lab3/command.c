#include "command.h"
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

void lfcat()
{
    /* Uses dirent.h and a struct to store file info */

    DIR *currentDirectory;
    char cwd[PATH_MAX];       /* Stores the cwd path: /usr/blabla */
    struct dirent *directory; /* Struct that holds file info */

    FILE* fName;
    
    char* line = NULL;
    size_t length = 0;
    FILE *input;

    if (getcwd(cwd, sizeof(cwd)) != NULL) /*  */
    {
        
        char* message = "<<In lfcat(): Step-01: Function called>>\n";
        char* messageTwo = "<<In lfcat(): Step-02: Listing all files in current dir.\n";
        // printf("Current working directory: %s\n", cwd);
        // write(1, message, strlen(message));
        currentDirectory = opendir(cwd);
        // write(1, messageTwo, strlen(messageTwo));
        FILE *fName = freopen("output.txt", "w+", stdout);

        while ((directory = readdir(currentDirectory)) != NULL)
        {            
            // write(1, "File: ", 6);
            // write(1, directory->d_name, strlen(directory->d_name));
            // write(1, "\n", 1);

            if (strcmp(directory->d_name, "1_poem.txt") == 0 || strcmp(directory->d_name, "2_lyrics.txt") == 0 || strcmp(directory->d_name, "3_DE_Code.py") == 0)
            {
                write(1, "File: ", 6);
                write(1, directory->d_name, strlen(directory->d_name));
                write(1, "\n", 1);
                input = fopen(directory->d_name, "r");
                while (getline(&line, &length, input) != -1)
                {
                    write(1, line, strlen(line));
                    write(1, "\n", 1);
                    // printf("%s", line);
                }
                // write(1, "\n", 1);
                for(int i = 0; i < 80; i++)
                    write(1, "-",1);
                write(1, "\n", 1);
            }
        }
    }
    else
    {
        printf("cwd error\n");
    }
    fclose(input);
    closedir(currentDirectory);
    exit(0);
}