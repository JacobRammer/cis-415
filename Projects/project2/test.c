#include <stdlib.h>
#include <string.h>
#include "header.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h> // added by me

// Globals for signal handler
int current_process_index;
int number_of_programs;
pid_t *pids;
int update_top;

// returns 1 if al processes have terminated
int allDone(pid_t *p, int len)
{
    int stat;
    int q = 0;
    for (q; q < number_of_programs; q++)
    {
        if (waitpid(p[q], &stat, WNOHANG) == 0)
        {
            return 0;
        }
    }
    return 1;
}

void signaler(int sig, pid_t *p_list, int num)
{
    int x = 0;
    for (x; x < num; x++)
    {
        kill(p_list[x], sig);
    }
}

void handler(int signum)
{
    // Nothing needs to be done in here yet
    // but SIGUSR1 will kill all the children if
    // it's not handled
}

int countSpaces(char *s)
{
    int len = strlen(s);
    int total = 0;
    int x = 0;
    for (x; x < len; x++)
    {
        if (s[x] == ' ')
        {
            total++;
        }
    }
    return total;
}

// assumes there will be no trailing spaces or anything after the last command
void removeNewline(char **s, size_t size)
{
    int len = strlen(*s);
    if ((*s)[len - 1] == '\n')
    {
        char *newString = malloc(size);
        int x = 0;
        memset(newString, '\0', size);
        for (x; x < len - 1; x++)
        {
            newString[x] = (*s)[x];
        }
        free(*s);
        *s = newString;
    }
}

int programCount(char *fn)
{
    char *b = NULL;
    size_t b_s;
    ssize_t c_r;
    int total = 0;
    FILE *f = fopen(fn, "r");

    c_r = getline(&b, &b_s, f);
    while (c_r != -1)
    {
        total++;
        c_r = getline(&b, &b_s, f);
    }

    free(b);
    fclose(f);
    return total;
}

void free2D(char **arr, int len)
{
    int x = 0;
    for (x; x < len; x++)
    {
        free(arr[x]);
    }
    free(arr);
}

void parentHandler(int signum)
{
    int checked = 0;
    int temp_status;
    if (!allDone(pids, number_of_programs))
    {
        //stop the currently running process
        kill(pids[current_process_index], SIGSTOP);
        current_process_index = (current_process_index + 1) % number_of_programs;
        // select next process
        while (waitpid(pids[current_process_index], &temp_status, WNOHANG) != 0)
        {
            checked++;
            current_process_index = (current_process_index + 1) % number_of_programs;
            if (checked > number_of_programs)
            {
                break;
            }
        }
        // start new process
        if (checked <= number_of_programs)
        {
            kill(pids[current_process_index], SIGCONT);
        }
    }
    else
    {
        printf("(HANDLER)SIGALRM recieved but all children have terminated. Suspending further alarms\n");
    }
    // print TOP-like info
    if (update_top % 2 == 0)
    { // update every 2 seconds
        //system("clear");
        printf("MCP:\n");
        int c = 0;
        printf("PID     NAME     S     %%CPU\n");
        for (c; c < number_of_programs; c++)
        {
            // vars
            int pid;
            char name[1000];
            char path[1000];
            char state;
            char DUMMY[1000];
            unsigned long utime;
            unsigned long stime;
            unsigned long pidcpu;
            unsigned int user;
            unsigned int nice;
            unsigned int system;
            unsigned int totalcpu;

            if (waitpid(pids[c], &temp_status, WNOHANG) == 0)
            {
                /*
					// I was told to look up how to calculate cpu usage so I'm citing the comment that I used to 
					// perform this calculation
				I'm not very sure this is the most correct way, but from a little browsing through "man proc" I'd say something like:
				- First get the PID of the process you want to get the CPU percentage of.
				- Then parse /proc/<PID>/stat and get the fields "utime" and "stime" from that file.
				- Add those two values and call the result "pidcpu", i.e.: pidcpu = utime + stime
				- Parse /proc/stat and add the first 3 fields from the first line ("cpu") together, call this result "totalcpu"
				- finally: percentage = 100 * pidcpu / totalcpu
				*/

                // read from /proc/[pid]/stat
                snprintf(path, 1000, "/proc/%d/stat", pids[c]);
                FILE *temp = fopen(path, "r");
                fscanf(temp, "%d %s %c %s %s %s %s %s %s %s %s %s %s %lu %lu ", &pid, name, &state, DUMMY, DUMMY, DUMMY, DUMMY, DUMMY, DUMMY, DUMMY, DUMMY, DUMMY, DUMMY, &utime, &stime);
                fclose(temp);

                // read from /proc/stat
                temp = fopen(path, "r");
                fscanf(temp, "cpu  %lu %lu %lu ", &user, &nice, &system);
                fclose(temp);

                // print full line
                pidcpu = utime + stime;
                totalcpu = user + nice + system;
                double cpu_percent = 100 * ((double)pidcpu / (double)totalcpu);

                printf("%d    %s    %c     %lf\n", pid, name, state, cpu_percent);
            }
        }
    }

    // reset alarm
    alarm(1);
    update_top++;
}

int main(int argc, char **argv)
{
    // declarations
    sigset_t signals;
    FILE *file;
    char *buffer;
    ssize_t bytes_read;
    size_t buffer_size;
    struct sigaction action;
    struct sigaction action2;
    int sig_recieved;
    int status;

    // assignments
    file = fopen(argv[1], "r");
    buffer = NULL;

    // global assignments
    current_process_index = 0;
    number_of_programs = programCount(argv[1]);
    pids = malloc(sizeof(pid_t) * number_of_programs);
    update_top = 0;

    //init signals for child process
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);

    // init new handler for SIGALRM
    action.sa_handler = parentHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;

    // init new handler for SIGUSR1 and maybe SIGALRM
    action2.sa_handler = handler;
    sigemptyset(&action2.sa_mask);
    //sigaddset(&action2.sa_mask, SIGALRM); // block SIGALRM
    action2.sa_flags = 0;

    int pid_index = 0;
    while (1)
    {
        // read line
        bytes_read = getline(&buffer, &buffer_size, file);
        if (bytes_read == -1)
        {
            break;
        }
        int number_of_args = countSpaces(buffer);
        removeNewline(&buffer, buffer_size);
        char *args[number_of_args + 2];
        args[0] = strtok(buffer, " ");
        int x = 0;
        for (x; x < number_of_args; x++)
        {
            args[x + 1] = strtok(NULL, " ");
        }
        args[number_of_args + 1] = NULL;

        // fork
        pids[pid_index] = fork();

        // failure
        if (pids[pid_index] < 0)
        {
            printf("ERROR\n");
            exit(EXIT_FAILURE);
        }
        // child code
        if (pids[pid_index] == 0)
        {
            sigaction(SIGUSR1, &action2, NULL);
            printf("(CHILD)Command waiting arg[0]: \"%s\"\n", args[0]);
            int check = sigwait(&signals, &sig_recieved);
            if (check > 0)
            {
                perror("ERROR");
            }
            printf("(CHILD)Command that recieved SIGCONT and is now entering exec arg[0]: \"%s\"\n", args[0]);
            execvp(args[0], args);
            exit(-1);
        }
        pid_index++;
        // NOTE: no parent branch is intended
    }

    sleep(1);

    printf("(PARENT)Completed forks.\n");

    // register alarm handler
    sigaction(SIGALRM, &action, NULL);

    int y = 0;
    for (y; y < number_of_programs; y++)
    {
        kill(pids[y], SIGUSR1);
    }
    y = 1;
    for (y; y < number_of_programs; y++)
    {
        kill(pids[y], SIGSTOP);
    }

    alarm(1);

    // The parent should just wait until all the children are done and only pause from waiting when it recieves the SIGALRM

    // WAIT
    printf("(PARENT)Waiting for all processes to terminate.\n");

    y = 0;
    for (y; y < number_of_programs; y++)
    {
        waitpid(pids[y], &status, 0);
    }
    printf("(PARENT)All processes have terminated. Closing...\n");

    // FREES
    free(buffer);
    free(pids);
    fclose(file);
}