// Lab 4 - Jacob Rammer

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

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

int main(int argc, char* argv[])
{
    int numChild = atoi(argv[1]);
    pid_t childPid[numChild];


    // int childPid = fork();
    int ret;
    char *arguments[] = {"iobound", "-seconds", "5", NULL};
    for(int i = 0; i < numChild; i++)
    {
        childPid[i] = fork();
        childPid[i] = execvp("./iobound", arguments);
        
    }
    // if(childPid == 0)    
    //     ret = execvp("./iobound", arguments);

   for(int i = 0; i < numChild; i++)
   {
       if(childPid[i] == 0)
           wait(0);
   }
        
    // printf("Hello world. PID:%d\n", getpid());

    return 0;
}