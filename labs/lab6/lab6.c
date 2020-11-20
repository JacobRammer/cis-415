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
}MealTicket;

typedef struct
{
    char name[MAXNAME];
    MealTicket* buffer;
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


int enqueue(char* MTQ_ID, MealTicket* MT)
{
    /*
    Enque a mealticket into the queue. 
    Return 1 if successful, else 0. 
    Need to find where in the registry our queue is
    */
    int registryIndex = -1;
    for(int i = 0; i < MAXQUEUES; i++)
    {
        char* temp = registry[i]->name;
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
        printf("Head is: %d\n", registry[registryIndex]->head);
        printf("pushing: Queue: <%s> - Ticket Number: <%d> - Dish: <%s>\n",
               registry[registryIndex]->name, registry[registryIndex]->buffer[registry[registryIndex]->head].ticketNum,
               registry[registryIndex]->buffer[registry[registryIndex]->head].dishName);
        registry[registryIndex]->head++;
        registry[registryIndex]->head = registry[registryIndex]->head % ENTRIES;
        registry[registryIndex]->length++;
        // printf("Head is: %d\n", registry[registryIndex]->head);
    }else  // queue is full
    {
        printf("%s is full. Nothing queued\n", registry[registryIndex]->name);
        return 0;
    }
}

int main()
{
    MTQ* Breakfast;
    MTQ* Lunch;
    MTQ* Dinner;
    MTQ* Bar;
    MealTicket breakfastTicket;
    MealTicket lunchTicket;
    MealTicket dinnerTicket;
    MealTicket barTicket;
    // breakfastTicket =  malloc(sizeof(MealTicket));
    // lunchTicket = malloc(sizeof(MealTicket));
    // dinnerTicket = malloc(sizeof(MealTicket));
    // barTicket = malloc(sizeof(MealTicket));


    Breakfast = malloc(sizeof(MTQ));
    Lunch = malloc(sizeof(MTQ));
    Dinner = malloc(sizeof(MTQ));
    Bar = malloc(sizeof(MTQ));
    Breakfast->buffer = malloc(sizeof(MealTicket) * 3);
    Lunch->buffer = malloc(sizeof(MealTicket) * 3);
    Dinner->buffer = malloc(sizeof(MealTicket) * 3);
    Bar->buffer = malloc(sizeof(MealTicket) * 3);

    Breakfast->head = 0;
    Breakfast->tail = 0;
    Lunch->head = 0;
    Lunch->tail = 0;
    Dinner->head = 0;
    Dinner->tail = 0;
    Bar->head = 0;
    Bar->tail = 0;

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

    for(int i = 0; i < 4; i++)
        printf("Registry[%d] name is: %s\n", i, registry[i]->name);
    enqueue("Breakfast", &breakfastTicket);
    enqueue("Breakfast", &breakfastTicket);
    enqueue("Breakfast", &breakfastTicket);
    enqueue("Breakfast", &breakfastTicket);
    enqueue("Breakfast", &breakfastTicket);
    // printf("First entry: %s\n", registry[0]->buffer[registry[0]->head].dishName);

    return 0;
}