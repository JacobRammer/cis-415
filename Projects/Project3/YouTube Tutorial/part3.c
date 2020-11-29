#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// The sum computed by the background thread
// long long sum = 0;

void* sum_runner(void* arg)
{
    /*
     * Thread function to generate sum of 0 to N
     */

    long long *limit_ptr = (long long *)arg;
    long long* limit = *limit_ptr;
    free(arg);

    long long sum = 0;
    for(long long i = 0; i <= limit; i++)
        sum += i;

    // Pass back date in dynamically allocated memory
    long long* answer = malloc(sizeof(*answer));
    *answer = sum;
    pthread_exit(answer);
}

int main(int argc, char** argv)
{

    if(argc < 2)
    {
        printf("Usage: %s <num>\n", argv[0]);
        exit(-1);
    }

    long long* limit = malloc(sizeof(*limit));
    *limit = atoll(argv[1]);
    // long long limit = atoll(argv[1]);


    // Thread ID
    pthread_t tID;

    // Create attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_create(&tID, &attr, sum_runner, limit);

    // Wait until thread is done its work
    long long* result;
    pthread_join(tID, &result);
    printf("Sum is %lld\n", *result);

    free(result);
    return 0;
}