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
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
int done = 1;
//=============================================================================

//============================ Structs and Macros =============================
typedef struct mealTicket{
	int ticketNum;
	char *dish;
} mealTicket;

//TODO: Declare a mutex in the struct. (e.g. add pthread_mutex_t ...)
typedef struct MTQ {
	char name[MAXNAME];
	struct mealTicket *buffer;
	int head;
	int tail;
	int max_length;
	int length;
	int ticket;
	pthread_mutex_t mutex;
} MTQ;


typedef struct
{
    char* name;
    mealTicket* MT;
    int threadID;
}pubStruct;

typedef struct
{
    char* name;
    mealTicket MT;
    int ticketNum;
    int thread;
}subStruct;

MTQ registry[MAXQUEUES];
pubStruct pubs[MAXPUBs];
subStruct subs[MAXSUBS];
pthread_t pubPthread[MAXPUBs];
pthread_t subPthread[MAXSUBS];

//TODO: Add a mutex init to this function (e.g registry[pos].mutex = PTHREAD_MUTEX_INITIALIZER)
//TODO: Add a conditional mutex, use pthread_cond_wait and pthread_cond_broadcast to make all threads hold until they are all ready to start together.

//A good example link is posted here
//https://www.geeksforgeeks.org/condition-wait-signal-multi-threading/

//It will be really helpeful for you to add a print entry function for debug and demonstration purpose.
void init(int pos, char *MTQ_ID) {
    printf("Initializing registry[%d] with name %s\n", pos, MTQ_ID);
//    registry[pos].mutex = PTHREAD_MUTEX_INITIALIZER;


    pthread_mutex_init(&registry[pos].mutex, NULL);
    registry[pos].buffer = malloc(sizeof(mealTicket) * MAXDISH);
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

void freeMTQ(int pos, char *MTQ_ID) {
    /*
    I don't think this is really needed?
    */
	free(registry[pos].buffer);
}
	

MTQ registry[MAXQUEUES]; //INFO: Changed to be an array of structs
//=============================================================================

//================================= Functions =================================
int enqueue(char *MTQ_ID, mealTicket *MT) {

    /*
   Enque a mealticket into the queue.
   Return 1 if successful, else 0.
   Need to find where in the registry our queue is
   */
    int registryIndex = -1;
    for(int i = 0; i < MAXQUEUES; i++)
    {
        // char* temp = registry[i].name;
        if(strcmp(MTQ_ID, registry[i].name) == 0)
        {
            registryIndex = i;
            break;
        }
    }

    if(registryIndex == -1)  // Queue name not found
        return 0;
    if(!isFull(registry[registryIndex]))
    {
        pthread_mutex_lock(&registry[registryIndex].mutex);
        registry[registryIndex].buffer[registry[registryIndex].head] = *MT;
        // printf("Head is: %d\n", registry[registryIndex].head);
        printf("pushing: Queue: <%s> - Ticket Number: <%d> - Dish: <%s>\n",
               registry[registryIndex].name, registry[registryIndex].buffer[registry[registryIndex].head].ticketNum,
               registry[registryIndex].buffer[registry[registryIndex].head].dish);
        registry[registryIndex].head++;
        registry[registryIndex].head = registry[registryIndex].head % MAXTICKETS;
        registry[registryIndex].length++;
        // printf("Length is %d\n", registry[registryIndex].length);
        // printf("Head is: %d\n", registry[registryIndex].head);
        pthread_mutex_unlock(&registry[registryIndex].mutex);
    }else  // queue is full
    {
        printf("pushing: Queue: <%s> - Error Queue is full.\n", registry[registryIndex].name);
        return 0;
    }

    return 1;
}

int dequeue(char *MTQ_ID, int ticketNum, mealTicket *MT) {

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
        // memcpy(MT, &registry[registryIndex].buffer[registry[registryIndex].tail], sizeof(mealTicket));
        MT->ticketNum = registry[registryIndex].buffer[registry[registryIndex].head].ticketNum;
        MT->dish = registry[registryIndex].buffer[registry[registryIndex].head].dish;
        printf("popping: Queue: <%s> - Ticket Number: <%d> - Dish: <%s>\n",
               registry[registryIndex].name, registry[registryIndex].buffer[registry[registryIndex].tail].ticketNum,
               registry[registryIndex].buffer[registry[registryIndex].tail].dish);
        registry[registryIndex].tail++;
        registry[registryIndex].tail = registry[registryIndex].tail % MAXTICKETS;
        registry[registryIndex].length--;
        pthread_mutex_unlock(&registry[registryIndex].mutex);
    }
    else
    {
        printf("popping: Queue <%s> - Queue is empty, nothing to pop\n", registry[registryIndex].name);
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

    pthread_mutex_lock(&mutex);
    int tID = ((pubStruct *)args)->threadID;
    // if (done == 1)
    // {
    //     printf("Thread %d waiting for signal.\n", tID);
    //     pthread_cond_wait(&cond1,&mutex);
    //     done
    // }else
    // {
        // get needed info from void* args
        // printf("TID is %d\n", tID);
        char *name = ((pubStruct *)args)->name;
        printf("Publisher: Queue <%s>, thread <%d>\n", name, tID);
        for (int i = 0; i < 3; i++)
        {
            mealTicket temp = ((pubStruct *)args)->MT[i];
            printf("Test: %s\n", temp.dish);
            // enqueue(name, &((pubStruct*) args)->MT)));
            enqueue(name, &temp);
            sleep(1);
        }
        pthread_mutex_unlock(&mutex);
    // }
    
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
    char *queue;
    int tID;
    int thread;
    mealTicket temp;

    queue = ((subStruct *)args)->name;
    tID = ((subStruct *)args)->ticketNum;
    thread = ((subStruct *)args)->thread;
    temp = ((subStruct *)args)->MT;
    // if (done == 1)
    // {
    //     printf("Thread %d waiting for signal.\n", tID);
    //     pthread_cond_wait(&cond1, &mutex);
    //     done = 2;
    // }else
    // {
        printf("Subscriber. Queue <%s> thread <%d>\n", queue, thread);
        dequeue(queue, tID, &temp);
        printf("Subscriber. Popped ticket <%d>: %s\n", temp.ticketNum, temp.dish);
    // }

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
	
	//STEP-1: Initialize the registry
	for(int i = 0; i < 4; i++)
    {
	    init(i, qNames[i]);
	    strcpy(registry[i].name, qNames[i]);
	    printf("Registry %s initialized \n", registry[i].name);
    }
//	printf("Is queue empty? %d\n", isEmpty(registry[0]));
//    printf("Is queue full? %d\n", isFull(registry[0]));
//    printf("Registry %s test \n", registry[0].name);
	
	//STEP-2: Create the publisher thread-pool
    // for(int i = 0; i < 4; i++)
    // {
    //     pubs[i].name = qNames[i];
    // }
    pubs[0].name = qNames[0];
    pubs[0].MT = bfast;
    pubs[0].threadID = 0;
    pthread_create(&pubPthread[0], NULL, publisher, (void*)&pubs[0]);
    // sleep(5);
    pubs[1].name = qNames[1];
    pubs[1].MT = lnch;
    pubs[1].threadID = 1;
    pthread_create(&pubPthread[1], NULL, publisher, (void*)&pubs[1]);
    // sleep(5);
    pubs[2].name = qNames[2];
    pubs[2].MT = dnr;
    pubs[2].threadID = 2;
    pthread_create(&pubPthread[2], NULL, publisher, (void*)&pubs[2]);
    // sleep(5);
    pubs[3].name = qNames[3];
    pubs[3].MT = br;
    pubs[3].threadID = 3;
    pthread_create(&pubPthread[3], NULL, publisher, (void*)&pubs[3]);
    sleep(1);
    //STEP-3: Create the subscriber thread-pool
    for(int i = 0; i < 4; i++)
    {
        mealTicket temp;
        subs[i].name = qNames[i];
        subs[i].ticketNum = registry[i].buffer[registry[i].head].ticketNum;
        subs[i].MT = temp;
        subs[i].thread = i;
        pthread_create(&subPthread[i], NULL, subscriber, (void*)&subs[i]);
    }

	//STEP-4: Join the thread-pools
    // done = 2;
    for(int i = 0; i < MAXPUBs; i++)
    {
        // done = 2;
        pthread_join(pubPthread[i], NULL);
        pthread_join(subPthread[i], NULL);
    }
	
	//STEP-5: Free the registry
    for(int i = 0; i < MAXQUEUES; i++)
    {
        free(registry[i].buffer);
    }
    return EXIT_SUCCESS;
    
}
//=============================================================================