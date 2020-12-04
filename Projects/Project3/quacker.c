#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/time.h>
//=============================================================================

//================================= Constants =================================
#define MAXNAME 15
#define MAXQUEUES 2
#define MAXTICKETS 3
#define MAXDISH 20
#define MAXPUBs 3
#define MAXSUBS 4
#define MAXURL 256
#define MAXCAPTION 256
#define MAXTOPIC 4  // max number of items in q

pthread_mutex_t globalMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t globalCond = PTHREAD_COND_INITIALIZER;

int delta = 1;

typedef struct
{
    int entryNum;
    struct timeval startTime;  // when the object was created
    struct timeval cleanTime;  // when the object is being looked at by cleanup
    int pubID;
    char photoURl[MAXURL];
    char photoCaption[MAXCAPTION];
}Topic;

typedef struct
{
    char name[MAXNAME];
    int head;
    int max;  // largest size the q can be
    int topicID;
    int tail;
    int length;
    int lastRead;
    int totalItems;  // what entry num will be. on every enque increment 
    Topic* buffer;
    pthread_mutex_t mutex;

}Queue;

typedef struct
{
    Topic* buffer;
    int id;
    int totalEntries;
}pubStruct;

typedef struct
{
    int lastEntry;
    Topic* buffer;
}subStruct;

Queue* registry[MAXQUEUES];  // stores all out q's


void initTopic(Topic* t, int id)
{
    t->entryNum = 0;
    t->pubID =  id;
    // clock_gettime(CLOCK_REALTIME, &t->timeStamp);
    gettimeofday(&t->startTime, NULL);
}

void initRegistry()
{
    for (int i = 0; i < MAXQUEUES; i++)
        registry[i]->topicID = -1;
}

void initQueue(Queue* q, char* qName, int topicType)
{   
    strcpy(q->name, qName);
    q->buffer = malloc(sizeof(Topic) * MAXTOPIC);
    q->head = 0;
    q->tail = 0;
    q->length = 0;
    q->totalItems = 0;
    q->topicID = topicType;
    q->lastRead = -1;
    q->max = 3;  // TODO fixme
    pthread_mutex_init(&q->mutex, NULL);
}

int isFull(Queue* q)
{
    return q->length == MAXTICKETS;
}

int isEmpty(Queue* q)
{
    return q->length == 0;
}

int findQueue(int topicType)
{
    int regIndex = -1;
    for (int i = 0; i < MAXQUEUES; i++)
    {
        if (registry[i]->topicID == topicType)
        {
            regIndex = i;
            break;
        }
    }

    return regIndex;
}

int enqueue(int topicType, Topic* t)
{
    int regIndex;
    regIndex = findQueue(topicType);
    // printf("Test: %d\n", regIndex);
    if(regIndex == -1)  // Queue not found
    {
        printf("From enqueue: Queue type not found\n");
        return 0;
    }    
    if(!isFull(registry[regIndex]))
    {
        pthread_mutex_lock(&registry[regIndex]->mutex);
        t->entryNum = registry[regIndex]->totalItems;
        registry[regIndex]->buffer[registry[regIndex]->head] = *t;
        registry[regIndex]->head++;
        registry[regIndex]->head = registry[regIndex]->head % MAXTICKETS;
        registry[regIndex]->length++;
        registry[regIndex]->totalItems++;
        printf("From enqueue: Successfully pushed <%d> into queue <%d>\n",
               t->entryNum, registry[regIndex]->topicID);
        pthread_mutex_unlock(&registry[regIndex]->mutex);
        return 1;
    }else
    {
        printf("From enqueue: Pushing: Queue: <%s> - Error queue is full.\n", 
            registry[regIndex]->name);
        return 0;
    }
    return 0;    
}

int dequeue(int topicID, Topic* t)
{
    int regIndex;
    regIndex = findQueue(topicID);
    if(regIndex == -1)
    {
        printf("From dequeue: queue type not found\n");
        return 0;
    }
    if(!isEmpty(registry[regIndex]))
    {
        pthread_mutex_lock(&registry[regIndex]->mutex);
        memcpy(t, &registry[regIndex]->buffer[registry[regIndex]->tail], sizeof(Topic));
        registry[regIndex]->tail++;
        registry[regIndex]->tail = registry[regIndex]->tail % MAXTICKETS;
        registry[regIndex]->length--;
        printf("From dequeue: Successfully popped <%d> from queue <%d>\n", 
        t->entryNum, registry[regIndex]->topicID);
        registry[regIndex]->lastRead = t->entryNum;
        printf("EntryNum %d\n", t->entryNum);
        printf("Last read: %d\n", registry[regIndex]->lastRead);
            pthread_mutex_unlock(&registry[regIndex]->mutex);
        return 1;
    }
    else
    {
        printf("From dequeue: popping: Queue: <%s> - Error queue is empty.\n",
               registry[regIndex]->name);
        return 0;
    }

    return 0;
}

int getEntry(int lastEntry, int topicType, Topic* t)
{
    /*
    Look a registry[topicType] and see the last read
    entry. Last read entry is pulled from a dequeued topic
    and is the entryNum for that topic. 
    */
    int location = findQueue(topicType);
    int max = registry[location]->max;
    int itter = registry[location]->max;
    printf("Location %d\n", location);
    pthread_mutex_lock(&registry[location]->mutex);

    // Case 1
    if(isEmpty(registry[location]))
        return 0;

    // Case 2
    // printf("Max: %d\n\n", itter);    
    for(int i = 0; i < itter; i++)
    {
        // printf("From entry: last read %d\n", registry[location]->lastRead);
        // printf("From entry: looking for tickerNum (lastread + 1): %d\n", lastEntry + 1);
        // printf("Test 3: %d\n", registry[location]->buffer[i].entryNum);
        int test3 = registry[location]->buffer[i].entryNum;
        int test4 = lastEntry + 1;
        if(registry[location]->buffer[i].entryNum == lastEntry + 1) 
            {
                printf("Case 2\n");
                memcpy(t, &registry[location]->buffer[i], sizeof(Topic));
                pthread_mutex_unlock(&registry[location]->mutex);
                return 1;
            }
            // printf("\n\n");
    }


    // Case 3a
    int ltFlag = 0;
    for (int i = 0; i < itter; i++)
    {
        int test = registry[location]->buffer[i].entryNum;
        int test2 = lastEntry + 1;
        if (registry[location]->buffer[i].entryNum <= lastEntry + 1)
        {
            ltFlag++;
            if(ltFlag == max)
            {
                printf("Case 3a\n");
                memcpy(t, &registry[location]->buffer[i], sizeof(Topic));
                pthread_mutex_unlock(&registry[location]->mutex);
                return 0;
            }
        }
    }

    // case 3b
    registry[location]->buffer[registry[location]->head].entryNum = 100;
    if(registry[location]->buffer[registry[location]->head].entryNum > lastEntry + 1)
    {
        printf("Case 3b\n");
        memcpy(t, &registry[location]->buffer[location], sizeof(Topic));
    }

    pthread_mutex_unlock(&registry[location]->mutex);
    return 0;
}

void cleanUp()
{
    for(int i = 0; i < MAXQUEUES; i++)
    {
        Topic *t1;
        t1 = malloc(sizeof(Topic));

        int found = findQueue(registry[i]->topicID);
        printf("found: %d\n", found);
        if(found != -1)
        { 
            int head = registry[i]->head;
            int tail = registry[i]->tail;
            for(int j = tail; j != head; j++)
            {
                char* name = registry[found]->buffer[j].photoCaption;
                sleep(5);
                gettimeofday(&registry[found]->buffer[j].cleanTime, NULL);
                long test = registry[found]->buffer[j].cleanTime.tv_sec - registry[found]->buffer[j].startTime.tv_sec;
                printf("Tail: %d, head: %d\n", tail, head);
                printf("Cleanup: %s\n", registry[found]->buffer[j].photoCaption);
                printf("Time test: %ld\n", test);
                if(test > delta)
                    dequeue(registry[found]->topicID, t1);
            }
        }
        free(t1);      
    }
}

void testQueue()
{
    printf("*** Testing queue data structure ***\n");
    Queue *q;
    q = malloc(sizeof(Queue));
    initQueue(q, "Test Queue", 1);
    registry[0] = q;

    Topic *t1;
    t1 = malloc(sizeof(Topic));
    initTopic(t1, 1);
    strcpy(t1->photoCaption, "T1 caption");
    strcpy(t1->photoURl, "T1 URL");

    Topic *t2;
    t2 = malloc(sizeof(Topic));
    initTopic(t2, 2);
    strcpy(t2->photoCaption, "T2 caption");
    strcpy(t2->photoURl, "T2 URL");

    Topic *t3;
    t3 = malloc(sizeof(Topic));
    initTopic(t3, 3);
    strcpy(t3->photoCaption, "T3 caption");
    strcpy(t3->photoURl, "T3 URL");

    // printf("Q <%s>, topic id <%d>\n", q->name, q->topicID);
    // printf("Registry 0 <%s>\n", registry[0]->name);
    // printf("Testing: %d\n", registry[0]->topicID);

    enqueue(1, t1);
    enqueue(1, t2);
    enqueue(1, t3);
    enqueue(1, t1);
    for (int i = 0; i < MAXTICKETS; i++)
    {
        printf("From main testing enqueue: publisher <%d> caption %s\n",
               registry[0]->buffer[i].pubID, registry[0]->buffer[i].photoCaption);
        // printf("Test entryNum: %d\n", registry[0]->buffer[i].entryNum);
    }

    // Topic *t4;
    // t4 = malloc(sizeof(Topic));

    // dequeue(1, t4);
    for (int i = 0; i < MAXTICKETS + 1; i++)
    {
        Topic *t4;
        t4 = malloc(sizeof(Topic));
        if (dequeue(1, t4))
            printf("From main testing dequeue: publisher <%d> caption %s\n",
                   t4->entryNum, t4->photoCaption);
    }
    // enqueue(1, t1);
}

void testEntry()
{
    printf("*** Testing entry function ***\n");
    Queue *q;
    q = malloc(sizeof(Queue));
    initQueue(q, "Test Queue", 1);
    registry[0] = q;

    Topic *t1;
    t1 = malloc(sizeof(Topic));
    initTopic(t1, 1);
    strcpy(t1->photoCaption, "T1 caption");
    strcpy(t1->photoURl, "T1 URL");

    Topic *t2;
    t2 = malloc(sizeof(Topic));
    initTopic(t2, 2);
    strcpy(t2->photoCaption, "T2 caption");
    strcpy(t2->photoURl, "T2 URL");

    Topic *t3;
    t3 = malloc(sizeof(Topic));
    initTopic(t3, 3);
    strcpy(t3->photoCaption, "T3 caption");
    strcpy(t3->photoURl, "T3 URL");

    // printf("Q <%s>, topic id <%d>\n", q->name, q->topicID);
    // printf("Registry 0 <%s>\n", registry[0]->name);
    // printf("Testing: %d\n", registry[0]->topicID);

    // t1->entryNum = 100;
    // t2->entryNum = 200;
    // t3->entryNum = 300;
    enqueue(1, t1);
    enqueue(1, t2);
    enqueue(1, t3);
    enqueue(1, t1);
    Topic *t4;
    t4 = malloc(sizeof(Topic));
    printf("Main test: %d\n", registry[0]->lastRead);
    
    dequeue(1, t4);  // last read is 0

    Topic* t5;
    t5 = malloc(sizeof(Topic));
    printf("From main: last read %d\n", registry[0]->lastRead);
    getEntry(registry[0]->lastRead, 1, t5);  // last entry should be 0
    printf("Testing lastEntry %s\n", t5->photoCaption);
    dequeue(1, t4);
    getEntry(registry[0]->lastRead, 1, t5); // last entry should be 0
    printf("Testing lastEntry %s\n", t5->photoCaption);
    registry[0]->lastRead = -2;
    printf("FML: %d\n", registry[0]->lastRead);
    getEntry(registry[0]->lastRead, 1, t5);
    printf("After modified last readEntry: Testing lastEntry %s\n", t5->photoCaption);
    // dequeue(1, t4); // last read is 1
    // dequeue(1, t4);  // last read is 2
    // dequeue(1, t4);  // queue empty, last read is 2
}

void testCleanup()
{
    Queue *q;
    q = malloc(sizeof(Queue));
    initQueue(q, "Test Queue", 1);
    registry[0] = q;

    Topic *t1;
    t1 = malloc(sizeof(Topic));
    initTopic(t1, 1);
    strcpy(t1->photoCaption, "T1 caption");
    strcpy(t1->photoURl, "T1 URL");

    Topic *t2;
    t2 = malloc(sizeof(Topic));
    initTopic(t2, 2);
    strcpy(t2->photoCaption, "T2 caption");
    strcpy(t2->photoURl, "T2 URL");

    Topic *t3;
    t3 = malloc(sizeof(Topic));
    initTopic(t3, 2);
    strcpy(t3->photoCaption, "T3 caption");
    strcpy(t3->photoURl, "T3 URL");

    Topic *t5;
    t5 = malloc(sizeof(Topic));

    Queue* q2;
    q2 = malloc(sizeof(Queue));
    initQueue(q2, "T2 Queue", 2);
    registry[1] = q2;

    enqueue(1, t1);
    enqueue(1, t2);
    enqueue(2, t3);
    printf("Test: %s\n", registry[1]->buffer[0].photoCaption);
    // dequeue(1, t5);
    cleanUp();
}

int main(int argc, char* argv[])
{
    // initRegistry();
    // testQueue();
    // testEntry(); 
    testCleanup();  

    return 0;
}