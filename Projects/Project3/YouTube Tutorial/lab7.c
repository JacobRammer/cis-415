#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define MAXPROX 10  // 10 threads

static int numBuf = 200;

pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t numBufMutex = PTHREAD_MUTEX_INITIALIZER;

void* foo(void* arg)
{
    int countDown = *((int*) arg);
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&condition, &mutex);
    pthread_mutex_unlock(&mutex);
    while(countDown != 0)
    {
        pthread_mutex_lock(&numBufMutex);
        printf("Thread ID: %ld\nCountDown: %d\nbuffer: %d\n", pthread_self(), countDown, numBuf);
        
        for(int i = 0; i < 5000; i++)
        {
            numBuf = numBuf - 2;
            numBuf = numBuf + 2;
        }
        numBuf = numBuf - 2;
        pthread_mutex_unlock(&numBufMutex);
        countDown--;
        sleep(1);
    }
}

typedef struct
{
    pthread_t tID;  // thread ID
    int flag; // init to 0, 1 = busy
}threadFlag;

int main()
{
    pthread_t testThread[MAXPROX];
    int countdown = 10;   // seconds

    for(int i = 0; i < MAXPROX; i++)
    {
        pthread_create(&(testThread[i]), NULL, foo, &countdown);
    }

    printf("5 seconds before start\n\n");
    sleep(5);

    pthread_mutex_lock(&mutex);
    // pthread_cond_broadcast(&condition);
    pthread_mutex_unlock(&mutex);

    for(int i = 0; i < MAXPROX; i++)
    {
        // wait for threads to quit
        // pthread_join(testThread[i], NULL);
    }

    printf("\n\nend numBuf is: %d\n", numBuf);

    return 0;
}