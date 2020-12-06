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
#define NUMPROXY 10  // per piazza

int numTopics = 0;
int queuesCreated = 0;

pthread_mutex_t globalMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t globalCond = PTHREAD_COND_INITIALIZER;

int delta = 10;

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
    int topicID;  // ie type of q like m"breakfast"
    int length;
}pubStruct;

typedef struct
{
    Topic buffer;
    int topicID;
    Topic t;
}subStruct;

Queue* registry;  // stores all our q's
pubStruct globalPubs[NUMPROXY / 2];
subStruct globalSubs[NUMPROXY / 2];
pthread_t pubThread[NUMPROXY / 2];
pthread_t subThread[NUMPROXY / 2];


Topic initTopic(Topic t, int id)
{
    // t = malloc(sizeof(Topic));
    t.entryNum = 0;
    t.pubID =  id;
    // clock_gettime(CLOCK_REALTIME, &t->timeStamp);
    gettimeofday(&t.startTime, NULL);

    return t;
}

void initRegistry()
{
    for (int i = 0; i < MAXQUEUES; i++)
        registry[i].topicID = -1;
}

void initQueue(char* qName, int topicType, int maxEntry)
{
    pthread_mutex_lock(&globalMutex);  // numTopics is a critical section
    strcpy(registry[queuesCreated].name, qName);
    registry[queuesCreated].buffer = malloc(sizeof(Topic) * MAXTOPIC);
    registry[queuesCreated].head = 0;
    registry[queuesCreated].tail = 0;
    registry[queuesCreated].length = 0;
    registry[queuesCreated].totalItems = 0;
    registry[queuesCreated].topicID = topicType;
    registry[queuesCreated].lastRead = -1;
    registry[queuesCreated].max = maxEntry;
    pthread_mutex_init(&registry[queuesCreated].mutex, NULL);
    pthread_mutex_unlock(&globalMutex);
    queuesCreated++;
    // numTopics++;
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
        if (registry[i].topicID == topicType)
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
    if(!isFull(&registry[regIndex]))
    {
        pthread_mutex_lock(&registry[regIndex].mutex);
        t->entryNum = registry[regIndex].totalItems;
        registry[regIndex].buffer[registry[regIndex].head] = *t;
        registry[regIndex].head++;
        registry[regIndex].head = registry[regIndex].head % MAXTICKETS;
        registry[regIndex].length++;
        registry[regIndex].totalItems++;
        printf("From enqueue: Successfully pushed <%d> into queue <%d>\n",
               t->entryNum, registry[regIndex].topicID);
        pthread_mutex_unlock(&registry[regIndex].mutex);
        return 1;
    }else
    {
        printf("From enqueue: Pushing: Queue: <%s> - Error queue is full.\n", 
            registry[regIndex].name);
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
    if(!isEmpty(&registry[regIndex]))
    {
        pthread_mutex_lock(&registry[regIndex].mutex);
        memcpy(t, &registry[regIndex].buffer[registry[regIndex].tail], sizeof(Topic));
        registry[regIndex].tail++;
        registry[regIndex].tail = registry[regIndex].tail % MAXTICKETS;
        registry[regIndex].length--;
        registry[regIndex].totalItems++;
        printf("From dequeue: Successfully popped <%d> from queue <%d>\n", 
        t->entryNum, registry[regIndex].topicID);
        registry[regIndex].lastRead = t->entryNum;
        printf("EntryNum %d\n", t->entryNum);
        printf("Last read: %d\n", registry[regIndex].lastRead);
            pthread_mutex_unlock(&registry[regIndex].mutex);
        return 1;
    }
    else
    {
        printf("From dequeue: popping: Queue: <%s> - Error queue is empty.\n",
               registry[regIndex].name);
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
    int max = registry[location].length;
    int itter = registry[location].max;
    // printf("Head: %d\n", registry[location].le);
    printf("Location %d\n", location);
    pthread_mutex_lock(&registry[location].mutex);

    // Case 1
    if(isEmpty(&registry[location]))
        return 0;

    // Case 2
    // printf("Max: %d\n\n", itter);   
    int tail = registry[location].tail; 
    for(int i = tail; i < itter; i++)
    {
        // printf("From entry: last read %d\n", registry[location].lastRead);
        // printf("From entry: looking for tickerNum (lastread + 1): %d\n", lastEntry + 1);
        // printf("Test 3: %d\n", registry[location].buffer[i].entryNum);
        int test3 = registry[location].buffer[i].entryNum;
        int test4 = lastEntry + 1;
        if(registry[location].buffer[i].entryNum == lastEntry + 1) 
            {
                printf("Case 2\n");
                memcpy(t, &registry[location].buffer[i], sizeof(Topic));
                pthread_mutex_unlock(&registry[location].mutex);
                return 1;
            }
            // printf("\n\n");
    }


    // Case 3a
    int ltFlag = 0;
    for (int i = tail; i < itter; i++)
    {
        int test = registry[location].buffer[i].entryNum;
        int test2 = lastEntry + 1;
        if (registry[location].buffer[i].entryNum <= lastEntry + 1)
        {
            ltFlag++;
            if(ltFlag == max)
            {
                printf("Case 3a\n");
                // memcpy(t, &registry[location].buffer[i], sizeof(Topic));
                pthread_mutex_unlock(&registry[location].mutex);
                return 0;
            }
        }
    }

    // case 3b
    registry[location].buffer[registry[location].head].entryNum = 100;
    if(registry[location].buffer[registry[location].head].entryNum > lastEntry + 1)
    {
        printf("Case 3b\n");
        memcpy(t, &registry[location].buffer[location], sizeof(Topic));
        return 1;
    }

    pthread_mutex_unlock(&registry[location].mutex);
    return 0;
}

void* cleanUp()
{
    printf("Cleanup function started\n");
    // sleep(2);  // TODO remove
    for (int i = 0; i < numTopics; i++)
    {
        Topic *t1;
        t1 = malloc(sizeof(Topic));

        int found = findQueue(registry[i].topicID);
        printf("found: %d\n", found);
        if(found != -1)
        { 
            int head = registry[i].head;
            int tail = registry[i].length;
            for(int j = head; j != tail; j++)  // TODO fix this. think this is good
            {
                char* name = registry[found].buffer[j].photoCaption;
                // sleep(5);
                gettimeofday(&registry[found].buffer[j].cleanTime, NULL);
                long test = registry[found].buffer[j].cleanTime.tv_sec - registry[found].buffer[j].startTime.tv_sec;
                printf("Tail: %d, head: %d\n", tail, head);
                printf("Cleanup: %s\n", registry[found].buffer[j].photoCaption);
                printf("Time test: %ld\n", test);
                if(test > delta)
                {
                    printf("Cleanup: old entry found. Removing ticket: %d %s\n",
                           registry[found].buffer[j].entryNum, registry[found].buffer[j].photoCaption);
                    dequeue(registry[found].topicID, t1);
                }
            }
        }
        free(t1);      
    }
    printf("Cleanup function finished\n");
}

void* publisher(void* args)
{
    int queueType = ((pubStruct*) args)->topicID;
    // printf("TopicID: %d\n", queueType);
    printf("Type publisher: Thread %ld. Waiting\n", pthread_self());
    pthread_mutex_lock(&globalMutex);
    pthread_cond_wait(&globalCond, &globalMutex);
    pthread_mutex_unlock(&globalMutex);
    int queueIndex; // store the queue type index 
    int numItems;  // So we can only itterate through tickets since each q has diff length
    queueIndex = findQueue(queueType);
    // numItems = globalPubs[queueIndex].length;
    numItems = ((pubStruct*) args)->length;
    int status = 0;
    for(int i = 0; i < numItems; i++)
    {
        Topic temp = ((pubStruct*) args)->buffer[i];
        printf("Publisher thread %ld enqueuing\n", pthread_self());
        status = enqueue(queueType, &temp);
        if(status == 0)
        {
            printf("From publisher: <%ld> Current queue is full. Waiting\n", pthread_self());
            while(status == 0)  // try until queue has a free spot
            {
                sched_yield();
                status = enqueue(queueType, &temp);
                break; // TODO remove me
            }
        }
    }
}

void* subscriber(void* args)
{
    int queueType = ((subStruct*) args)->topicID;
    printf("Type subscriber: Thread %ld. Waiting\n", pthread_self());
    pthread_mutex_lock(&globalMutex);
    pthread_cond_wait(&globalCond, &globalMutex);
    pthread_mutex_unlock(&globalMutex);
    Topic temp = ((subStruct*) args)->t;
    int dq = 1;
    while(dq)
    {
        // perror("HI");
        dq = dequeue(queueType, &temp);
        if(dq == 1)
        {
            printf("Subscriber: queue type <%d>, popped entry num %d. Thread %ld\n",
                   queueType, temp.entryNum, pthread_self());
        }else
        {
            printf("Subscriber: Queue %d is empty. Thread %ld\n", queueType, pthread_self());
            sleep(1);
            // sched_yield();
            dq = dequeue(queueType, &temp);
        }
        
    }
}

void getTopic(char *line)
{
    int returnVal = 0;
    int bufferSize = 256;
    // char *lineBuffer;
    char *savePtr;
    int index = 0;
    // lineBuffer = (char *)malloc(bufferSize * sizeof(char));
    char *topicNum = strtok_r(line, " ", &savePtr);
    topicNum = strtok_r(NULL, " ", &savePtr);
    char* name = strtok_r(NULL, " ", &savePtr);
    int length = strlen(name);
    char qName[length];
    memset(qName, '\0', sizeof(char) * length);
    for (int i = 0; i < length; i++)
    {
        char tem = name[i];
        if (name[i] != '"' && name[i] != '\n')
        {
            strncat(qName, &name[i], 1);
        }
    }
    char* max = strtok_r(NULL, " ", &savePtr);
    int newMax = atoi(max);
    int newTopicNum = atoi(topicNum);
    printf("From getTopic: %d, %s, %d\n", newTopicNum, qName, newMax);
    initQueue(qName, newTopicNum, newMax);
}

int numQueues(FILE* file, int mode)
{
    /*
    Gets the number of queues needed to be created from the input file. 
    Number of queues can be found by going line by line and counting
    "create topic"
    */

    int returnVal = 0;
    int bufferSize = 256;
    char* lineBuffer;
    char* savePtr;
    int index = 0;
    lineBuffer = (char*)malloc(bufferSize * sizeof(char));
    while(fgets(lineBuffer, bufferSize, file) != NULL)
    {
        char* command = strtok_r(lineBuffer, " ", &savePtr);
        if(strcmp(command, "create") == 0)
        {
            returnVal++;
            numTopics++;
            if(mode == 1)
                getTopic(savePtr);
        }
    }
    free(lineBuffer);
    return returnVal;
}

int numPublishers(FILE* file)
{
    /*
    Gets the number of publishers from the input file. 
    This can be denoted as "add publisher""
    */

    int publisher = 0;
    int bufferSize = 256;
    char* lineBuffer;
    char* savePtr;
    lineBuffer = (char*)malloc(sizeof(char) * bufferSize);
    while(fgets(lineBuffer, bufferSize, file) != NULL)
    {
        char* temp = strtok_r(lineBuffer, " ", &savePtr);
        temp = strtok_r(NULL, " ", &savePtr);
        printf("Temp: %s\n", temp);
        if(strcmp(temp, "publisher") == 0)
        {
            publisher++;
        }
    }
    free(lineBuffer);
    return publisher;
}

void findDelta(FILE* file)
{
    /*
    Goes through the file and finds the delta time
    */

    int bufferSize = 256;
    char* lineBuffer;
    char* savePtr;
    lineBuffer = (char*)malloc(sizeof(char) * bufferSize);
    while(fgets(lineBuffer, bufferSize, file) != NULL)
    {
        char* temp = strtok_r(lineBuffer, " ", &savePtr);
        if(strcmp(temp, "delta") == 0)
        {
            temp = strtok_r(NULL, " ", &savePtr);
            delta = atoi(temp);
            printf("Delta is: %d\n", delta);
        }
    }
    free(lineBuffer);
}

void addPubArray(FILE* file, char** array)
{
    /*
    Add publishers and subscribers to a array so we
    can open up those files later
    */
   int bufferSize = 256;
   char* lineBuffer;
   char* savePtr;
   int index = 0;
   lineBuffer = (char*)malloc(sizeof(char) * bufferSize);
   while(fgets(lineBuffer, bufferSize, file) != NULL)
   {
       char* temp = strtok_r(lineBuffer, " ", &savePtr);
       temp = strtok_r(NULL, " ", &savePtr);
       if(strcmp(temp, "publisher") == 0) 
       {
           temp = strtok_r(NULL, " ", &savePtr);
           int length = strlen(temp);
           char newFileName[length];
           memset(newFileName, '\0', sizeof(char) * length);
           for(int i = 0; i < length; i++)
           {
                char tem = temp[i];   
               if(temp[i] != '"' && temp[i] != '\n')
               {
                   strncat(newFileName, &temp[i], 1);
               } 
           }
        //    printf("Temp is: %s\n", temp);
            printf("Filename is: %s\n", newFileName);
            strcpy(array[index], newFileName);
            index++;
       }
   }
   free(lineBuffer);
}

void addSubArray(FILE* file, char** array)
{
    int bufferSize = 256;
    char *lineBuffer;
    char *savePtr;
    int index = 0;
    lineBuffer = (char *)malloc(sizeof(char) * bufferSize);
    while (fgets(lineBuffer, bufferSize, file) != NULL)
    {
        char *temp = strtok_r(lineBuffer, " ", &savePtr);
        temp = strtok_r(NULL, " ", &savePtr);
        if (strcmp(temp, "subscriber") == 0)
        {
            temp = strtok_r(NULL, " ", &savePtr);
            int length = strlen(temp);
            char newFileName[length];
            memset(newFileName, '\0', sizeof(char) * length);
            for (int i = 0; i < length; i++)
            {
                char tem = temp[i];
                if (temp[i] != '"' && temp[i] != '\n')
                {
                    strncat(newFileName, &temp[i], 1);
                }
            }
            //    printf("Temp is: %s\n", temp);
            printf("Filename is: %s\n", newFileName);
            strcpy(array[index], newFileName);
            index++;
        }
    }
    free(lineBuffer);
}


int main(int argc, char* argv[])
{
    // initRegistry();
    // testQueue();
    // testEntry(); 
    // testCleanup();
    // testPubSub();  

    FILE* fName;
    int numCommands;
    int numPubs;

    fName = fopen(argv[1], "r");
    if(fName == NULL)
    {
        printf("No valid file: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    numCommands = numQueues(fName, 0);
    registry = malloc(sizeof(Queue) * numCommands);

    Topic t1;
    initTopic(t1, 1);
    strcpy(t1.photoCaption, "T1 caption");
    strcpy(t1.photoURl, "T1 URL");

    Topic t2;
    initTopic(t2, 2);
    strcpy(t2.photoCaption, "T2 caption");
    strcpy(t2.photoURl, "T2 URL");

    fName = fopen(argv[1], "r");
    numQueues(fName, 1);
    enqueue(1, &t1);
    
    printf("Num of queues needed: %d\n", numCommands);
    fName = fopen(argv[1], "r");
    numPubs = numPublishers(fName);
    fName = fopen(argv[1], "r");
    findDelta(fName);
    printf("Num pubs: %d\n", numPubs);
    fName = fopen(argv[1], "r");
    char* temp;
    char *publisherArray[numPubs];
    char* subscriberArray[numPubs];
    for(int i = 0; i < numPubs; i++)
    {
        publisherArray[i] = (char *)malloc(sizeof(char) & 256);
        subscriberArray[i] = (char *)malloc(sizeof(char) & 256);
    }
    addPubArray(fName, publisherArray);
    fName = fopen(argv[1], "r");
    addSubArray(fName, subscriberArray);

    // read input file to create proper q
    fName = fopen(argv[1], "r");
    // Queue* q;
    // initQueue(1, "Quacker Queue")

    
    // publisherArray[0] = "Test";
    // publisherArray[1] = "Test1";
    // publisherArray[2] = "Test2";
    printf("Final test: %s\n", publisherArray[0]);
    printf("Final test: %s\n", subscriberArray[0]);

    return 0;
}