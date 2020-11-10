#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int commands(FILE *file)
{
    /*
    Counts the number of commands in a file. 
    A command occupies one line of a test file. 
    So this function basically counts the number of
    lines in a file
    */

    int returnVal = 0;
    int bufferSize = 256;
    char *lineBuffer;
    lineBuffer = (char *)malloc(bufferSize * sizeof(char));
    while (fgets(lineBuffer, bufferSize, file) != NULL)
    {
        returnVal++;
    }

    free(lineBuffer);
    fclose(file);
    return returnVal;
}

void removeNewline(char *input)
{
    int length = strlen(input);
    // printf("Input is %s\n", input);
    for (int i = 0; i < length; i++)
    {
        char temp = input[i];
        if (strcmp(&input[i], "\n") == 0)
        {
            input[i] = '\0';
        }
    }

    // printf("Return string: %s\n", input);
}

int arguments(char *line)
{
    /*
    Counts the nunber of arguments per line. An argument
    defined here is a command seperated by a space
    */

    int args = 0;
    int length = strlen(line);
    for (int i = 0; i < length; i++)
    {
        char temp = line[i];
        if (line[i] == ' ')
            args++;
    }
    return (args + 1);
}

void signaler(pid_t *pool, int numChildren, int signal, int wait)
{
    sleep(wait);
    // printf("Num children: %d\n", numChildren);
    for (int i = 0; i < numChildren; i++)
    {
        printf("Parent process: <%d> - Sending signal: <%d> to child"
               "process: <%d>\n",
               getpid(), signal, pool[i]);
        kill(pool[i], signal);
    }
}

void handler(int sig)
{
    // printf("From sigHandle Child Process: <%d> - Received signal: SIGUSR1 - Calling exec().\n", getpid());
}

void script_print(pid_t *pid_ary, int size)
{
    FILE *fout;
    fout = fopen("top_script.sh", "w");
    fprintf(fout, "#!/bin/bash\ntop");
    for (int i = 0; i < size; i++)
    {
        fprintf(fout, " -p %d", (int)(pid_ary[i]));
    }
    fprintf(fout, "\n");
    fclose(fout);
}

int childRunning(pid_t* pool, int numChildren)
{
    /*
    Checks to see if any process is still running.
    Returns 1 (True) if they are else 0 (false)
    */

   int status; 
   for(int i = 0; i < numChildren; i++)
   {
        if(waitpid(pool[i], &status, WNOHANG) == 0)
            return 0;  // not done
        return 1;  // all done
   }
}

void alarmSignal()
{
    printf("Alarm signal\n");
}

int main(int argc, char *argv[])
{

    FILE *fName;
    int numCommands; // how many commands ie # of lines in file

    fName = fopen(argv[1], "r");
    if (fName == NULL)
    {
        printf("No valid file: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // Get number of commands
    numCommands = commands(fName);

    size_t bufferSize = 256;
    char *lineBuffer; // hold line with newline
    lineBuffer = (char *)malloc(bufferSize * sizeof(char));

    ssize_t characters = 0;
    fName = fopen(argv[1], "r");
    characters = getline(&lineBuffer, &bufferSize, fName);
    removeNewline(lineBuffer);
    pid_t childArray[numCommands];
    char *savePtr;
    int currentChild = 0; // Saves which command we're on

    /*
    Lab 5 variables
    */
    sigset_t signalSet; // per IBM docs. Var was "signal" need to rename
    int sigNumber;

    sigemptyset(&signalSet);
    sigaddset(&signalSet, SIGUSR1);

    struct sigaction action = {0}; // Per IBM docs
    action.sa_handler = handler;
    sigaction(SIGUSR1, &action, NULL);

    while (characters > 0)
    {
        // printf("LineBuffer: %s\n", lineBuffer);
        int numArgs = arguments(lineBuffer);
        char *argumentArray[numArgs + 1]; // for null
        argumentArray[numArgs] = NULL;
        char *tempCommand = strtok_r(lineBuffer, " ", &savePtr);
        for (int i = 0; i < numArgs; i++)
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
        if (childArray[currentChild] == 0)
        {
            printf("Child process: <%d> - Waiting for SIGUSR1..\n", getpid());
            sigwait(&signalSet, &sigNumber);
            printf("Child Process: <%d> - Received signal: SIGUSR1 - Calling exec().\n", getpid());
            if(childArray[currentChild] = execvp(argumentArray[0], argumentArray) < 0);
            {
                perror("Execvp error: ");
                exit(0);
            }
            // exit(0);
        }

        // Move onto the next command (line in file)
        characters = getline(&lineBuffer, &bufferSize, fName);
        removeNewline(lineBuffer);
        currentChild++;
    }
    script_print(childArray, numCommands);
    printf("\nSending children to SIGURS1\n");
    signaler(childArray, numCommands, SIGUSR1, 0);

    printf("\nCalling exec and sending children to SIGSTOP\n");
    signaler(childArray, numCommands, SIGSTOP, 0);

    /*
    Here is where we need to loog through all the processes
    until they're done for 1 second each
    */

    // sleep(5);
    // printf("\nSending children to SIGCONT\n");
    // signaler(childArray, numCommands, SIGCONT, 3);
    // sleep(3);
    // signaler(childArray, numCommands, SIGINT, 3);
    printf("\n\n!!!!!!Checking to see if any child has exited!!!!!!\n\n");
    signal(SIGALRM, alarmSignal);
    alarm(5);

    int status;
    for (int i = 0; i < numCommands; i++)
        while (waitpid(childArray[i], &status, 0) > 0)
            ;
    free(lineBuffer);
    fclose(fName);
    printf("\nAll processes are done executing.\n");
    return 0;
}