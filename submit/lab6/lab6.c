#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXNAME 50
#define MAXQUEUES 4
#define ENTRIES 3; // max entries


typedef struct
{
    int ticketNum;
    char* dishName;
}mealTicket;

typedef struct
{
    char name[MAXNAME];
    mealTicket* buffer;
    int head;
    int tail;
    int length;
}MTQ;

MTQ *registry[MAXQUEUES];

int isFull(MTQ* queue)
{
    /*
    Checks to see if the queue is full.
    Return 1 if full, else 0
    */

   return queue->length >= ENTRIES;
}

int isEmpty(MTQ* queue)
{
    /*
    Checks to see if a queue is empty. 
    Returns 1 is empty, else 0
    */

   return queue->length == 0;
}


int enqueue(char* MTQ_ID, mealTicket* MT)
{
    /*
    Enque a mealticket into the queue. 
    Return 1 if successful, else 0. 
    Need to find where in the registry our queue is
    */
    int registryIndex = -1;
    for(int i = 0; i < MAXQUEUES; i++)
    {
        // char* temp = registry[i]->name;
        if(strcmp(MTQ_ID, registry[i]->name) == 0)
        {
            registryIndex = i;
            break;
        }
    }

    if(registryIndex == -1)  // Queue name not found
        return 0;
    if(!isFull(registry[registryIndex]))
    {
        registry[registryIndex]->buffer[registry[registryIndex]->head] = *MT;
        // printf("Head is: %d\n", registry[registryIndex]->head);
        printf("pushing: Queue: <%s> - Ticket Number: <%d> - Dish: <%s>\n",
               registry[registryIndex]->name, registry[registryIndex]->buffer[registry[registryIndex]->head].ticketNum,
               registry[registryIndex]->buffer[registry[registryIndex]->head].dishName);
        registry[registryIndex]->head++;
        registry[registryIndex]->head = registry[registryIndex]->head % ENTRIES;
        registry[registryIndex]->length++;
        // printf("Length is %d\n", registry[registryIndex]->length);
        // printf("Head is: %d\n", registry[registryIndex]->head);
    }else  // queue is full
    {
        printf("pushing: Queue: <%s> - Error Queue is full.\n", registry[registryIndex]->name);
        return 0;
    }

    return 1;
}

int dequeue(char *MTQ_ID, mealTicket *MT)
{
    /*
    Dequeue a ticket from the tail. Returns
    1 if successful, else 0
    */

   int registryIndex = -1;
   for(int i = 0; i < MAXQUEUES; i++)
   {
       if(strcmp(MTQ_ID, registry[i]->name) == 0)
       {
           registryIndex = i;
           break;
       }
   }

   if(registryIndex == -1)
       return 0;

    if(!isEmpty(registry[registryIndex]))
    {
        memcpy(MT, &registry[registryIndex]->buffer[registry[registryIndex]->tail], sizeof(mealTicket));
        printf("popping: Queue: <%s> - Ticket Number: <%d> - Dish: <%s>\n",
               registry[registryIndex]->name, registry[registryIndex]->buffer[registry[registryIndex]->tail].ticketNum,
               registry[registryIndex]->buffer[registry[registryIndex]->tail].dishName);
        registry[registryIndex]->tail++;
        registry[registryIndex]->tail = registry[registryIndex]->tail % ENTRIES;
        registry[registryIndex]->length--;
    }else
    {
        printf("popping: Queue <%s> - Queue is empty, nothing to pop\n", registry[registryIndex]->name);
    }

    return 1;
    
}

int main()
{
    MTQ* Breakfast;
    MTQ* Lunch;
    MTQ* Dinner;
    MTQ* Bar;
    mealTicket breakfastTicket;
    mealTicket lunchTicket;
    mealTicket dinnerTicket;
    mealTicket barTicket;
    mealTicket* popTicketOne; // to test dequeue function
    mealTicket* popTicketTwo;
    mealTicket* popTicketThree;
    // breakfastTicket =  malloc(sizeof(mealTicket));
    // lunchTicket = malloc(sizeof(mealTicket));
    // dinnerTicket = malloc(sizeof(mealTicket));
    // barTicket = malloc(sizeof(mealTicket));


    Breakfast = malloc(sizeof(MTQ));
    Lunch = malloc(sizeof(MTQ));
    Dinner = malloc(sizeof(MTQ));
    Bar = malloc(sizeof(MTQ));
    Breakfast->buffer = malloc(sizeof(mealTicket) * 3);
    Lunch->buffer = malloc(sizeof(mealTicket) * 3);
    Dinner->buffer = malloc(sizeof(mealTicket) * 3);
    Bar->buffer = malloc(sizeof(mealTicket) * 3);
    popTicketOne = malloc(sizeof(mealTicket));
    popTicketTwo = malloc(sizeof(mealTicket));
    popTicketThree = malloc(sizeof(mealTicket));

    Breakfast->head = 0;
    Breakfast->tail = 0;
    Breakfast->length = 0;
    Lunch->head = 0;
    Lunch->tail = 0;
    Lunch->length = 0;
    Dinner->head = 0;
    Dinner->tail = 0;
    Dinner->length = 0;
    Bar->head = 0;
    Bar->tail = 0;
    Bar->length = 0;

    breakfastTicket.dishName = "Eggs";
    breakfastTicket.ticketNum = 0;

    lunchTicket.dishName = "Turkey Sandwhich";
    lunchTicket.ticketNum = 1;

    dinnerTicket.dishName = "Steak";
    dinnerTicket.ticketNum = 2;

    barTicket.dishName = "Beer";
    barTicket.ticketNum = 3;

    registry[0] = Breakfast;
    registry[1] = Lunch;
    registry[2] = Dinner;
    registry[3] = Bar;
    strcpy(Breakfast->name, "Breakfast");
    strcpy(Lunch->name, "Lunch");
    strcpy(Dinner->name, "Dinner");
    strcpy(Bar->name, "Bar");

    // dequeue("Breakfast", popTicket);

    // for(int i = 0; i < 4; i++)
    //     printf("Registry[%d] name is: %s\n", i, registry[i]->name);

    printf("Testing queue function with max size 3\n\n");
    enqueue("Breakfast", &breakfastTicket);
    enqueue("Breakfast", &lunchTicket);
    enqueue("Breakfast", &dinnerTicket);
    enqueue("Breakfast", &barTicket);

    enqueue("Lunch", &breakfastTicket);
    enqueue("Lunch", &lunchTicket);
    enqueue("Lunch", &dinnerTicket);
    enqueue("Lunch", &barTicket);

    enqueue("Dinner", &breakfastTicket);
    enqueue("Dinner", &lunchTicket);
    enqueue("Dinner", &dinnerTicket);
    enqueue("Dinner", &barTicket);

    enqueue("Bar", &breakfastTicket);
    enqueue("Bar", &lunchTicket);
    enqueue("Bar", &dinnerTicket);
    enqueue("Bar", &barTicket);
    printf("\nDone testing queue function\n\n");

    printf("Testing dequeue function\n\n");
    for(int i = 0; i < 3; i++)
    {
        dequeue("Breakfast", popTicketOne);
        dequeue("Lunch", popTicketTwo);
        dequeue("Dinner", popTicketThree);
        dequeue("Bar", popTicketTwo);
    }
    printf("\nPopping from empty queue\n\n");
    dequeue("Breakfast", popTicketOne);
    dequeue("Lunch", popTicketTwo);
    dequeue("Dinner", popTicketThree);
    dequeue("Bar", popTicketTwo);
    // printf("\nDone testing dequeue function\n\n");
    // printf("PopTicketOne contents: %s - %d\n", popTicketOne->dishName, popTicketOne->ticketNum);
    // printf("PopTicketTwo contents: %s - %d\n", popTicketTwo->dishName, popTicketTwo->ticketNum);
    // printf("PopTicketThree contents: %s - %d\n", popTicketThree->dishName, popTicketThree->ticketNum);

    // printf("First entry: %s\n", registry[0]->buffer[registry[0]->head].dishName);

    free(Breakfast->buffer);
    free(Lunch->buffer);
    free(Dinner->buffer);
    free(Bar->buffer);
    free(Breakfast);
    free(Lunch);
    free(Dinner);
    free(Bar);
    free(popTicketOne);
    free(popTicketTwo);
    free(popTicketThree);
    return 0;
}