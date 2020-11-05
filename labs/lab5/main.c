#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>


void signaler(pid_t* pool, int numChildren, int signal)
{
    for (int i = 0; i < numChildren; i++)
    {
        printf("Parent process: <%d> - Sending signal: <%d> to child"
         "process: <%d>\n", getpid(), signal, pool[i]);
         kill(pool[i], signal);
         sleep(2);
    }
}

void signalHandler(int sig)
{
    printf("Child Process: <%d> - Received signal: <%d> - Calling exec().\n", getpid(), sig);
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

    struct sigaction action;
    action.sa_handler = signalHandler;
    sigaction(SIGUSR1, &action, NULL);

    for(int i = 0; i < numChild; i++)
    {
        childPid[i] = fork();
        if(childPid[i] == 0)
        {
            printf("Child process: <%d> - Waiting for SIGUSR1..\n", getpid());
            sigwait(&signal, &sigNumber);
            printf("Child Process: <%d> - Received signal: <%d> - Calling exec().\n", getpid(), sigNumber);
            // exit(0);
            char *arguments[] = {"iobound", "-seconds", "5", NULL};
            childPid[i] = execvp("./iobound", arguments);        
        }
    }

    sleep(4); // per lab
    script_print(childPid, numChild);
    signaler(childPid, numChild, SIGUSR1);
    signaler(childPid, numChild, SIGSTOP);
    sleep(5);
    signaler(childPid, numChild, SIGCONT);
    sleep(3);
    signaler(childPid, numChild, SIGINT);

    wait(0);  //TODO change to waitpid
    return 0;
}
