#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/wait.h>
//=============================================================================

//================================= Constants =================================
#define MAXNAME 15
#define MAXQUEUES 4
#define MAXTICKETS 3
#define MAXDISH 20
#define MAXPUBs 3
#define MAXSUBS 4
#define MAXURL 256
#define MAXCAPTION 256
#define MAXTOPIC 4  // max number of items in q

pthread_mutex_t globalMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t globalCond = PTHREAD_COND_INITIALIZER;

typedef struct
{
    int entryNum;
    struct timeval timeStamp;
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

int findQueue(Topic* t, int topicType)
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
}

int enqueue(int topicType, Topic* t)
{
    int regIndex;
    regIndex = findQueue(t, topicType);
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
    regIndex = findQueue(t, topicID);
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

    pthread_mutex_lock(&registry[topicType]->mutex);

    // Case 1
    if(isEmpty(registry[topicType]))
        return 0;

    // Case 2
    int itter = registry[topicType]->max;
    for(int i = 0; i < itter; i++)
    {
        if(registry[topicType]->buffer[registry[itter]])
    }
}

void testQueue()
{
    printf("*** Testing queue data structure ***");
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

    t1->entryNum = 100;
    t2->entryNum = 200;
    t3->entryNum = 300;
    enqueue(1, t1);
    enqueue(1, t2);
    enqueue(1, t3);
    enqueue(1, t1);
    Topic *t4;
    t4 = malloc(sizeof(Topic));
    dequeue(1, t4);  // last read is 0
    dequeue(1, t4); // last read is 1
    dequeue(1, t4);  // last read is 2
    dequeue(1, t4);  // queue empty, last read is 2
}

int main(int argc, char* argv[])
{
    // testQueue();
    testEntry();   

    return 0;
}