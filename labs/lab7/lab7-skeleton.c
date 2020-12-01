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
pthread_mutex_t globalMutex;
pthread_cond_t globalCond;
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
    mealTicket** buffer;
    char* name[MAXDISH];
    int tID;
    int length;
}pubStruct;

typedef struct
{
    char* name;
    mealTicket* MT;
    int tID;
}subStruct;

MTQ* registry[MAXQUEUES];
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
    
    // printf("Test: %d, Name: %s\n", pos, MTQ_ID);
    registry[pos]->buffer = malloc(sizeof(mealTicket) * MAXTICKETS);
    pthread_mutex_init(&registry[pos]->mutex, NULL);
    strcpy(registry[pos]->name, MTQ_ID);
    registry[pos]->head = 0;
    registry[pos]->tail = 0;
    registry[pos]->length = 0;
}

int isEmpty(MTQ* queue)
{
    /*
    Checks to see if a queue is empty.
    Returns 1 is empty, else 0
    */

    return queue->length == 0;
}

int isFull(MTQ* queue)
{
    /*
    Checks to see if the queue is full.
    Return 1 if full, else 0
    */

    return queue->length >= MAXTICKETS;
}

void freeMTQ(int pos, char *MTQ_ID) {
    /*
    I don't think this is really needed?
    */
	
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
        // char* temp = registry[i]->name;
        if (strcmp(MTQ_ID, registry[i]->name) == 0)
        {
            registryIndex = i;
            break;
        }
    }

    if (registryIndex == -1) // Queue name not found
        return 0;
    if (!isFull(registry[registryIndex]))
    {
        registry[registryIndex]->buffer[registry[registryIndex]->head] = *MT;
        // printf("Head is: %d\n", registry[registryIndex]->head);
        printf("pushing: Queue: <%s> - Ticket Number: <%d> - Dish: <%s>\n",
               registry[registryIndex]->name, registry[registryIndex]->buffer[registry[registryIndex]->head].ticketNum,
               registry[registryIndex]->buffer[registry[registryIndex]->head].dish);
        registry[registryIndex]->head++;
        registry[registryIndex]->head = registry[registryIndex]->head % MAXDISH;
        registry[registryIndex]->length++;
        // printf("Length is %d\n", registry[registryIndex]->length);
        // printf("Head is: %d\n", registry[registryIndex]->head);
    }
    else // queue is full
    {
        printf("pushing: Queue: <%s> - Error Queue is full.\n", registry[registryIndex]->name);
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
        if (strcmp(MTQ_ID, registry[i]->name) == 0)
        {
            registryIndex = i;
            break;
        }
    }

    if (registryIndex == -1)
        return 0;

    if (!isEmpty(registry[registryIndex]))
    {
        memcpy(MT, &registry[registryIndex]->buffer[registry[registryIndex]->tail], sizeof(mealTicket));
        printf("popping: Queue: <%s> - Ticket Number: <%d> - Dish: <%s>\n",
               registry[registryIndex]->name, registry[registryIndex]->buffer[registry[registryIndex]->tail].ticketNum,
               registry[registryIndex]->buffer[registry[registryIndex]->tail].dish);
        registry[registryIndex]->tail++;
        registry[registryIndex]->tail = registry[registryIndex]->tail % MAXDISH;
        registry[registryIndex]->length--;
    }
    else
    {
        printf("popping: Queue <%s> - Queue is empty, nothing to pop\n", registry[registryIndex]->name);
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
    
    pubStruct* pub = (pubStruct*) args;
    printf("Thread: Publisher. Thread ID: %ld waiting for signal\n", pthread_self());
    pthread_cond_wait(&globalCond, &globalMutex);
    pthread_mutex_unlock(&globalMutex);

    for (int i = 0; i < pub->length; i++)
    {
        enqueue(pub->name[i], pub->buffer[i]);
        sleep(1);
    }
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
    
    subStruct* sub = (subStruct*) args;
    printf("Thread: Subscriber. Thread ID: %ld waiting for signal\n", pthread_self());
    pthread_cond_wait(&globalCond, &globalMutex);
    pthread_mutex_unlock(&globalMutex);
    while(!dequeue(sub->name, sub->MT))
    {
        printf("Thread %ld popped %s\n", pthread_self(), sub->MT->dish);
    }

    printf("Thread %ld. Queue empty, nothing to pop\n", pthread_self());
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
    MTQ* Breakfast = malloc(sizeof(MTQ));
    MTQ* Lunch = malloc(sizeof(MTQ));
    MTQ* Dinner = malloc(sizeof(MTQ));
    MTQ* Bar = malloc(sizeof(MTQ));
    registry[0] = Breakfast;
    registry[1] = Lunch;
    registry[2] = Dinner;
    registry[3] = Bar;

    for(int i = 0; i < MAXQUEUES; i++)
    {
        init(i, qNames[i]);
        printf("Name Test: %s\n", registry[i]->name);
        // printf("Test: ")
    }

    
	
	//STEP-2: Create the publisher thread-pool
    pubStruct* pubBreakfast = malloc(sizeof(pubStruct));
    pubBreakfast->buffer = malloc(sizeof(mealTicket) * MAXDISH);
    pubBreakfast->buffer[0] = &bfast[0];
    pubBreakfast->buffer[1] = &bfast[1];
    pubBreakfast->buffer[2] = &bfast[2];
    pubBreakfast->name[0] = "Breakfast";
    pubBreakfast->name[1] = "Breakfast";
    pubBreakfast->name[2] = "Breakfast";
    pubBreakfast->length = 3;

    pubStruct* pubLunch = malloc(sizeof(pubStruct));
    pubLunch->buffer = malloc(sizeof(mealTicket) * MAXDISH);
    pubLunch->buffer[0] = &lnch[0];
    pubLunch->buffer[1] = &lnch[1];
    pubLunch->buffer[2] = &lnch[2];
    pubLunch->name[0] = "Lunch";
    pubLunch->name[1] = "Lunch";
    pubLunch->name[2] = "Lunch";
    pubLunch->length = 3;

    pubStruct *pubDinner = malloc(sizeof(pubStruct));
    pubDinner->buffer = malloc(sizeof(mealTicket) * MAXDISH);
    pubDinner->buffer[0] = &dnr[0];
    pubDinner->buffer[1] = &dnr[1];
    pubDinner->buffer[2] = &dnr[2];
    pubDinner->name[0] = "Dinner";
    pubDinner->name[1] = "Dinner";
    pubDinner->name[2] = "Dinner";
    pubDinner->length = 3;

    pubStruct *pubBar = malloc(sizeof(pubStruct));
    pubBar->buffer = malloc(sizeof(mealTicket) * MAXDISH);
    pubBar->buffer[0] = &br[0];
    pubBar->buffer[1] = &br[1];
    pubBar->buffer[2] = &br[2];
    pubBar->name[0] = "Bar";
    pubBar->name[1] = "Bar";
    pubBar->name[2] = "Bar";
    pubBar->length = 3;

    pthread_cond_init(&globalCond, NULL);
    pthread_mutex_init(&globalMutex, NULL);

    pthread_t pBreakfast;
    pthread_t pLunch;
    pthread_t pDinner;
    pthread_t pBar;
    pthread_create(&pBreakfast, NULL, publisher, pubBreakfast);
    pthread_create(&pLunch, NULL, publisher, &pubLunch);
    pthread_create(&pDinner, NULL, publisher, pubDinner);
    pthread_create(&pBar, NULL, publisher, pubBar);

    sleep(1); // make sure threads have started
    pthread_cond_broadcast(&globalCond);

    //STEP-3: Create the subscriber thread-pool
    subStruct* subBreakfast = malloc(sizeof(subStruct));
    subBreakfast->name = "Breakfast";
    subBreakfast->MT = malloc(sizeof(mealTicket));
    subStruct* subLunch = malloc(sizeof(subStruct));
    subLunch->name = "Lunch";
    subLunch->MT = malloc(sizeof(mealTicket));
    subStruct* subDinner = malloc(sizeof(subStruct));
    subDinner->name = "Dinner";
    subDinner->MT = malloc(sizeof(mealTicket));
    subStruct* subBar = malloc(sizeof(subStruct));
    subBar->name = "Bar";
    subBar->MT = malloc(sizeof(mealTicket));

    pthread_t sBreakfast;
    pthread_t sLunch;
    pthread_t sDinner;
    pthread_t sBar;
    pthread_create(&sBreakfast, NULL, subscriber, subBreakfast);
    pthread_create(&sLunch, NULL, subscriber, subLunch);
    pthread_create(&sDinner, NULL, subscriber, subDinner);
    pthread_create(&sBar, NULL, subscriber, subBar);
    sleep(1);
    pthread_cond_broadcast(&globalCond);
    //STEP-4: Join the thread-pools
    pthread_join(pBreakfast, NULL);
    pthread_join(pLunch, NULL);
    pthread_join(pDinner, NULL);
    pthread_join(pBar, NULL);
    pthread_join(sBreakfast,  NULL);
    pthread_join(sLunch, NULL);
    pthread_join(sDinner, NULL);
    pthread_join(sBar, NULL);

	//STEP-5: Free the registry

    return EXIT_SUCCESS;
    
}
//=============================================================================