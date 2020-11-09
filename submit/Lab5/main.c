#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

/*
Jacob Rammer
Per the Piazza post, we wait 3 seconds before calling our signaler
function. When I do that, once of the prcesses finish before it's 
terminated. I'm going to assume that's fine. If I increase
the run time of io bound, the issue goes away but
I'm not gonna mess with it
*/

void signaler(pid_t* pool, int numChildren, int signal, int wait)
{
    sleep(wait);
    for (int i = 0; i < numChildren; i++)
    {
        printf("Parent process: <%d> - Sending signal: <%d> to child"
         "process: <%d>\n", getpid(), signal, pool[i]);
         kill(pool[i], signal);
    }
}

void handler(int sig)
{
    printf("From sigHandle Child Process: <%d> - Received signal: SIGUSR1 - Calling exec().\n", getpid());
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

int main()
{
    int numChild = 5;
    pid_t childPid[numChild];
    sigset_t signal;  // per IBM docs
    int sigNumber;
    
    sigemptyset(&signal);
    sigaddset(&signal, SIGUSR1);

    struct sigaction action = {0};  // Per IBM docs
    action.sa_handler = handler;
    sigaction(SIGUSR1, &action, NULL);

    //TODO look at this to possibly change this in the project
    // https://github.com/nsf/termbox/issues/35

    for(int i = 0; i < numChild; i++)
    {
        childPid[i] = fork();
        if(childPid[i] == 0)
        {
            printf("Child process: <%d> - Waiting for SIGUSR1..\n", getpid());
            sigwait(&signal, &sigNumber);
            printf("Child Process: <%d> - Received signal: SIGUSR1 - Calling exec().\n", getpid());
            // exit(0);
            char *arguments[] = {"iobound", "-seconds", "5", NULL};
            childPid[i] = execvp("./iobound", arguments);        
        }
    }

    // sleep(4); // per lab
    script_print(childPid, numChild);
    signaler(childPid, numChild, SIGUSR1, 2);
    signaler(childPid, numChild, SIGSTOP, 3);
    // sleep(5);
    signaler(childPid, numChild, SIGCONT, 3);
    // sleep(3);
    signaler(childPid, numChild, SIGINT, 3);

    int status;
    int counter = 0;
    for(int i = 0; i < numChild; i++)
        while(waitpid(childPid[i], &status, 0) > 0);
    return 0;
}
