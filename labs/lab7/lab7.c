/*=============================================================================
 * Program Name: lab7
 * Author: Jared Hall
 * Edited by Grayson Guan
 * Date: 11/17/2020
 * Description:
 *     A simple program that implements a thread-safe queue of meal tickets
 *
 *===========================================================================*/

/*
Yeah this lab doesn't work worth jack. Big fat 0 :(
*/

//========================== Preprocessor Directives ==========================
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
pthread_mutex_t globalMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t globalCond = PTHREAD_COND_INITIALIZER;
//=============================================================================

//============================ Structs and Macros =============================
typedef struct {
	int ticketNum;
	char *dish;
} mealTicket;

typedef struct  {
	char name[MAXNAME];
 mealTicket *buffer;
	int head;
	int tail;
	int max_length;
	int length;
	int ticket;
	pthread_mutex_t mutex;
} MTQ;


typedef struct
{
    mealTicket* buffer;
    char* name;
    int tID;
    int length;
}pubStruct;

typedef struct
{
    char* name;
    mealTicket MT;
    int tID;
    unsigned long thread;
}subStruct;

MTQ registry[MAXQUEUES];
pubStruct globalPubs[MAXPUBs];
subStruct globalSubs[MAXSUBS];
pthread_t pubPthread[MAXPUBs];
pthread_t subPthread[MAXSUBS];

//TODO: Add a mutex init to this function (e.g registry[pos].mutex = PTHREAD_MUTEX_INITIALIZER)
//TODO: Add a conditional mutex, use pthread_cond_wait and pthread_cond_broadcast to make all threads hold until they are all ready to start together.

//A good example link is posted here
//https://www.geeksforgeeks.org/condition-wait-signal-multi-threading/

//It will be really helpeful for you to add a print entry function for debug and demonstration purpose.
void init(int pos, char *MTQ_ID) {
    
    // printf("Test: %d, Name: %s\n", pos, MTQ_ID);
    registry[pos].buffer = malloc(sizeof(mealTicket) * MAXTICKETS);
    pthread_mutex_init(&registry[pos].mutex, NULL);
    strcpy(registry[pos].name, MTQ_ID);
    registry[pos].head = 0;
    registry[pos].tail = 0;
    registry[pos].length = 0;
}

int isEmpty(MTQ queue)
{
    /*
    Checks to see if a queue is empty.
    Returns 1 is empty, else 0
    */

    return queue.length == 0;
}

int isFull(MTQ queue)
{
    /*
    Checks to see if the queue is full.
    Return 1 if full, else 0
    */

    return queue.length >= MAXTICKETS;
}

void freeMTQ(int pos) {
    // printf("hi\n");
    free(registry[pos].buffer);
	
}
	
//=============================================================================

//================================= Functions =================================
int enqueue(char *MTQ_ID, mealTicket *MT) {

    /*
    Enque a mealticket into the queue. 
    Return 1 if successful, else 0. 
    Need to find where in the registry our queue is
    */
    int registryIndex = -1;
    for (int i = 0; i < MAXQUEUES; i++)
    {
        // char* temp = registry[i].name;
        if (strcmp(MTQ_ID, registry[i].name) == 0)
        {
            registryIndex = i;
            break;
        }
    }

    if (registryIndex == -1) // Queue name not found
        return 0;
    pthread_mutex_lock(&registry[registryIndex].mutex);
    if (!isFull(registry[registryIndex]))
    {
        registry[registryIndex].buffer[registry[registryIndex].head] = *MT;
        // printf("Head is: %d\n", registry[registryIndex].head);
        printf("pushing: Queue: <%s> - Ticket Number: <%d> - Dish: <%s>\n",
               registry[registryIndex].name, registry[registryIndex].buffer[registry[registryIndex].head].ticketNum,
               registry[registryIndex].buffer[registry[registryIndex].head].dish);
        registry[registryIndex].head++;
        registry[registryIndex].head = registry[registryIndex].head % MAXDISH;
        registry[registryIndex].length++;
        // printf("Length is %d\n", registry[registryIndex].length);
        // printf("Head is: %d\n", registry[registryIndex].head);
        pthread_mutex_unlock(&registry[registryIndex].mutex);
    }
    else // queue is full
    {
        printf("pushing: Queue: <%s> - Error Queue is full.\n", registry[registryIndex].name);
        return 0;
    }

    return 1;
}

int dequeue(char *MTQ_ID, mealTicket *MT) {

    /*
    Dequeue a ticket from the tail. Returns
    1 if successful, else 0
    */

    int registryIndex = -1;
    for (int i = 0; i < MAXQUEUES; i++)
    {
        if (strcmp(MTQ_ID, registry[i].name) == 0)
        {
            registryIndex = i;
            break;
        }
    }

    if (registryIndex == -1)
        return 0;

    if (!isEmpty(registry[registryIndex]))
    {
        pthread_mutex_lock(&registry[registryIndex].mutex);
        memcpy(MT, &registry[registryIndex].buffer[registry[registryIndex].tail], sizeof(mealTicket));
        printf("popping: Queue: <%s> - Ticket Number: <%d> - Dish: <%s>\n",
               registry[registryIndex].name, registry[registryIndex].buffer[registry[registryIndex].tail].ticketNum,
               registry[registryIndex].buffer[registry[registryIndex].tail].dish);
        registry[registryIndex].tail++;
        registry[registryIndex].tail = registry[registryIndex].tail % MAXDISH;
        registry[registryIndex].length--;
        pthread_mutex_unlock(&registry[registryIndex].mutex);
    }
    else
    {
        printf("popping: Queue <%s> - Queue is empty, nothing to pop\n", registry[registryIndex].name);
        return 0;
    }

    return 1;
}

void *publisher(void *args) {
	/* TODO: The publisher will receive the following in the struct args:
	*        1. An array of mealTickets to push to the queue.
	*        2. For each meal ticket, an MTQ_ID. (Note: a publisher can push to multiple queues)
	*        3. The thread ID
	* The publisher will do the pthread_cond_wait procedure, and wait for a cond signal to begin its work
	* The publisher will then print its type and thread ID on startup. Then it will push one meal ticket at a time to
	* its appropriate queue before sleeping for 1 second. It will do this until there are no more meal tickets to push.
	*/

    char* queue = ((pubStruct*) args)->name;
    printf("Type publisher. Thread %ld. Waiting\n", pthread_self());
    pthread_mutex_lock(&globalMutex);
    pthread_cond_wait(&globalCond, &globalMutex);
    pthread_mutex_unlock(&globalMutex);
    // printf("Type publisher. Thread %ld\n", pthread_self());
    for(int i = 0; i < MAXTICKETS; i++)
    {
        mealTicket temp = ((pubStruct*) args)->buffer[i];
        printf("Publisher thread %ld enqueuing\n", pthread_self());
        enqueue(queue, &temp);
        sleep(1);
    }
    // pthread
}

void *subscriber(void *args) {
	/* TODO:The subscriber will take the following:
	*       1. The MTQ_ID it will pull from.
	*       2. The thread ID
	*       3. An empty meal ticket.
	* The subscriber will do the pthread_cond_wait procedure, and wait for a cond signal to begin its work
	* The subscriber will print its type and thread ID on startup. Then it will pull a ticket from its queue
	* and print it. If the queue is empty then it will print an empty message along with its
	* thread ID and wait for 1 second. If the thread is not empty then it will pop a ticket and 
	* print it along with the thread id.
	*/

    char* queue = ((subStruct*) args)->name;
    printf("Type subscriber. Thread %ld. Waiting\n", pthread_self());
    pthread_mutex_lock(&globalMutex);
    pthread_cond_wait(&globalCond, &globalMutex);
    pthread_mutex_unlock(&globalMutex);
    // printf("Type subscriber. Thread %ld\n", pthread_self());
    printf("Publisher thread %ld \n", pthread_self());

    mealTicket temp = ((subStruct*) args)->MT;
    char* name = ((subStruct*) args)->name;
    int dq = 1; // run first time reguardless
    while(dq)
    {
        dq = dequeue(name, &temp);
        if(dq == 1)
        {
            printf("Subscriber: queue: %s, popped dish %s. Thread %ld\n", name, temp.dish, pthread_self());
        }
        else
        {
            printf("Subscriber: Queue %s is empty. Thread: %ld\n", name, pthread_self());
            sleep(1);
            dq = dequeue(name, &temp);
        }
        
    }

    // dequeue(name, &temp);
    // printf("Subscriber: queue: %s, dish %s\n", name, temp.dish);
    // dequeue(name, &temp);
    // printf("Subscriber: queue: %s, dish %s\n", name, temp.dish);
}
//=============================================================================

//=============================== Program Main ================================
int main(int argc, char argv[]) {
	//Variables Declarations
	char *qNames[] = {"Breakfast", "Lunch", "Dinner", "Bar"};
	char *bFood[] = {"Eggs", "Bacon", "Steak"};
	char *lFood[] = {"Burger", "Fries", "Coke"};
	char *dFood[] = {"Steak", "Greens", "Pie"};
	char *brFood[] = {"Whiskey", "Sake", "Wine"};
	int i, j, t = 1;
	int test[4];
	char dsh[] = "Empty";
	mealTicket bfast[3] = {[0].dish = bFood[0], [1].dish = bFood[1], [2].dish = bFood[2]};
	mealTicket lnch[3] = {[0].dish = lFood[0], [1].dish = lFood[1], [2].dish = lFood[2]};
	mealTicket dnr[3] = {[0].dish = dFood[0], [1].dish = dFood[1], [2].dish = dFood[2]};
	mealTicket br[3] = {[0].dish = brFood[0], [1].dish = brFood[1], [2].dish = brFood[2]};
	mealTicket ticket = {.ticketNum=0, .dish=dsh};
    mealTicket testTicket;
    // strcpy(testTicket.dish, "Beer");
    testTicket.ticketNum = 1;
	
	//STEP-1: Initialize the registry  
    for(int i = 0; i <= MAXTICKETS; i++)
    {
        init(i, qNames[i]);
        printf("Initialized registry[%d]: <%s>\n", i, registry[i].name);
    }
	
	//STEP-2: Create the publisher thread-pool
    globalPubs[0].name = qNames[0];
    globalPubs[0].buffer = bfast;
    globalPubs[1].name = qNames[1];
    globalPubs[1].buffer = lnch;
    globalPubs[2].name = qNames[2];
    globalPubs[2].buffer = dnr;
    globalPubs[3].name = qNames[3];
    globalPubs[3].buffer = br;

    // Now create pthreads
    for(int i = 0; i < MAXQUEUES; i++)
    {
        pthread_create(&pubPthread[i], NULL, publisher, &globalPubs[i]);
    }
    // pthread_create(&pubPthread[0], NULL, publisher, &globalPubs[0]);

    sleep(1);
    // pthread_cond_broadcast(&globalCond);
    // sleep(2);
    // pthread_join(pubPthread[0], NULL);
    // for(int i = 0; i < MAXTICKETS; i++)
    // {
    //     printf("Test: Queue <%s>, dish<%s>\n", globalPubs[i].name, globalPubs[i].buffer[i].dish);
    // }

    // for(int i = 0; i < 4; i++)
    // {
    //     printf("globalPubs[%d]: %s. Buffer[0]: %s\n", i, globalPubs[i].name, globalPubs[i].buffer[1].dish);
    // }

    //STEP-3: Create the subscriber thread-pool
    for(int i = 0; i < MAXQUEUES; i++)
    {
        mealTicket temp;
        globalSubs[i].name = qNames[i];
        globalSubs[i].MT = temp;
        pthread_create(&subPthread[i], NULL, subscriber, &globalSubs[i]);
    }
    sleep(1);
    // pthread_create(&subPthread[0], NULL, subscriber, &globalSubs[0]);
    printf("Starting threads in 1 second.\n");
    sleep(1);
    pthread_cond_broadcast(&globalCond);

    // pthread_join(subPthread[0], NULL);

    //STEP-4: Join the thread-pools
    for(int i = 0; i <= MAXPUBs; i++)
    {
        pthread_join(pubPthread[i], NULL);
        pthread_join(subPthread[i], NULL);
    }
    

    //STEP-5: Free the registry
    for(int i = 0; i < MAXQUEUES; i++)
        freeMTQ(i);

    // sleep(2);
    return EXIT_SUCCESS;
    
}
//=============================================================================