#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// The sum computed by the background thread
// long long sum = 0;

typedef struct
{
    long long limit;
    long long answer;
}sumRunner;


void* sum_runner(void* arg)
{
    /*
     * Thread function to generate sum of 0 to N
     */

    sumRunner* argStruct = (sumRunner*) arg;
    long long sum = 0;

    for(long long i = 0; i <= argStruct->limit; i++)
        sum += i;
    argStruct->answer = sum;
    pthread_exit(0);
}

int main(int argc, char** argv)
{

    if(argc < 2)
    {
        printf("Usage: %s <num1> <num2> ... <num-n>\n", argv[0]);
        exit(-1);
    }

    int numArgs = argc - 1;
    sumRunner args[numArgs];

    // Thread ID
    pthread_t tIDs[numArgs];
    for(int i = 0; i < numArgs; i++)
    {
        
        args[i].limit = atoll(argv[i + 1]);
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tIDs[i], &attr, sum_runner, &args[i]);
    }

    // Wait until thread is done its work
    for(int i = 0; i < numArgs; i++)
    {
        pthread_join(tIDs[i], NULL);
        printf("Sum for thread %ld is %lld\n", tIDs[i], args[i].answer);
    }
    
    

    return 0;
}