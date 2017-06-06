/*
 * CS61C Summer 2016
 * Name: Qingning Yao & Zihao Chen
 * Login: cs61c-bo & cs61c-ec
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "flights.h"
#include "timeHM.h"

struct flightSys {
  struct airport *head;//pointing to the head of the linked list of airports.
};

struct airport {
  char* airport_name;
  struct schedule* schdl; //schedule is the type of struct, schd is the variable name for the pointer to schedules. Here it means pointing to the head of this schdl linked list.
  struct airport *next; //linked list structure
};

struct schedule{
  char* destination_name;
  timeHM_t departure_time;
  timeHM_t arrival_time;
  struct schedule* next; //linked list structure
  int cost;
};

/*
   This should be called if memory allocation failed.
 */
static void allocation_failed() {
    fprintf(stderr, "Out of memory.\n");
    exit(EXIT_FAILURE);
}


/*
   Creates and initializes a flight system, which stores the flight schedules of several airports.
   Returns a pointer to the system created.
 */
flightSys_t* createSystem() {
    struct flightSys* s = calloc(1,sizeof(struct flightSys));
    if (s == NULL){
      allocation_failed();
    }
    return s;
}


/*
   Frees all memory associated with this system; that's all memory you dynamically allocated in your code.
 */
void deleteSystem(flightSys_t* s) {
  //Here it should be traversing down the linked list, deleting every airport including all of their schedules linked list as their elements
  struct airport *tempap = s->head;//creating a tempAirPort pointer for traversing down the linked list of airports
  struct airport *tempdelap;//used for deleting airports
  struct schedule *tempsd;//creating a tempScheDule pointer for traversing down each linked list of schedules
  struct schedule *tempdelsd;//used for deleting schedules
  while (tempap!=NULL){
    tempsd = tempap->schdl;
    free(tempap->airport_name);
    while (tempsd!=NULL){
      free(tempsd->destination_name);
      tempdelsd = tempsd;
      tempsd = tempsd->next;
      free(tempdelsd);
    }
    tempdelap = tempap;
    tempap = tempap->next;
    free(tempdelap);
  }
  free(s);
}


/*
   Adds a airport with the given name to the system. You must copy the string and store it.
   Do not store "name" (the pointer) as the contents it point to may change.
 */
 void addAirport(flightSys_t* s, char* name) {
     struct airport* ap = calloc(1,sizeof(struct airport));
     if (ap==NULL){
       allocation_failed();
     }
     ap->airport_name = calloc(1,sizeof(char*));
     strcpy(ap->airport_name, name);
     ap -> next = NULL;

     /*struct schedule* sd = calloc(1,sizeof(struct schedule));
     if (sd==NULL)
       allocation_failed();
     ap -> schdl = sd;*/

     struct airport* traverse = s->head;
     if(traverse == NULL){
       s->head = ap;
     }//finding the end of the airport linked list in s
     else {
         while (traverse->next!=NULL){
             traverse = traverse->next;
         }
         traverse->next = ap;
     }
 }


/*
   Returns a pointer to the airport with the given name.
   If the airport doesn't exist, return NULL.
 */
//traversing down the linked list of airport
airport_t* getAirport(flightSys_t* s, char* name) {
    struct airport* temp = s->head;//use a pointer temp to point to the beginning of the airport linked list, also used for traversing it
    if (temp==NULL) {
      return NULL;
    }
    while(temp!=NULL){
      if (strcmp(temp->airport_name, name)==0) {
        return temp;
      }
      temp = temp->next;
    }
    return NULL;
}


/*
   Print each airport name in the order they were added through addAirport, one on each line.
   Make sure to end with a new line. You should compare your output with the correct output
   in flights.out to make sure your formatting is correct.
 */
void printAirports(flightSys_t* s) {
    struct airport* temp = s->head;//using a temp pointer to point to the beginning of the airport linked list, used for later traversing
    while (temp!=NULL){
      printf("%s\n",temp->airport_name);
      temp = temp->next;
    }
}


/*
   Adds a flight to src's schedule, stating a flight will leave to dst at departure time and arrive at arrival time.
 */
void addFlight(airport_t* src, airport_t* dst, timeHM_t* departure, timeHM_t* arrival, int cost) {
    struct schedule *newschdl = calloc(1,sizeof(struct schedule));
    if (newschdl==NULL)
      allocation_failed();
    newschdl -> departure_time = *departure;
    newschdl -> arrival_time = *arrival;
    newschdl -> next = NULL;
    newschdl -> destination_name = calloc(1,sizeof(char*));
    newschdl -> cost = cost;
    if (newschdl -> destination_name == NULL)
      allocation_failed();
    strcpy(newschdl->destination_name, dst->airport_name);
    //copying the correspondin values into the new temp schedule struct created

    struct schedule *temp = src->schdl;
    if (temp == NULL){
      src->schdl = newschdl;
    }
    else{
      while (temp -> next!=NULL){
        temp = temp->next;
      }//finding the end of schedule linked list
      temp->next = newschdl;
    }
}


/*
   Prints the schedule of flights of the given airport.

   Prints the airport name on the first line, then prints a schedule entry on each
   line that follows, with the format: "destination_name departure_time arrival_time $cost_of_flight".

   You should use printTime (look in timeHM.h) to print times, and the order should be the same as
   the order they were added in through addFlight. Make sure to end with a new line.
   You should compare your output with the correct output in flights.out to make sure your formatting is correct.
 */
void printSchedule(airport_t* s) {
    struct airport *tempap = s;//creating a tempAirPort pointer for traversing down the linked list of airports
    struct schedule *tempsd;//creating a tempScheDule pointer for traversing down each linked list of schedules
    printf("%s\n",tempap->airport_name);
    tempsd = tempap->schdl;
    while (tempsd!=NULL){
      printf("%s ",tempsd->destination_name);
      printTime(&(tempsd->departure_time));
      printf(" ");
      printTime(&(tempsd->arrival_time));
      printf(" $%d\n",tempsd->cost);
      tempsd = tempsd->next;
    }
}


/*
   Given a src and dst airport, and the time now, finds the next flight to take based on the following rules:
   1) Finds the cheapest flight from src to dst that departs after now.
   2) If there are multiple cheapest flights, take the one that arrives the earliest.

   If a flight is found, you should store the flight's departure time, arrival time, and cost in departure, arrival,
   and cost params and return true. Otherwise, return false.

   Please use the function isAfter() from time.h when comparing two timeHM_t objects.
 */

 //this version is Qingning's getNextFlight(), as an alternative to Zihao's version
bool getNextFlight(airport_t* src, airport_t* dst, timeHM_t* now, timeHM_t* departure, timeHM_t* arrival, int* cost) {
  struct schedule *tempsd;//creating a tempScheDule pointer for traversing down each linked list of schedules
  tempsd = src->schdl;
  int current_lowest_cost = 2147483647;
  struct schedule *choice = NULL;//the current best choice

  while (tempsd!=NULL){
    if (strcmp(tempsd->destination_name, dst->airport_name)==0){//if the name of the destination airport matches the name of our destination
      if (isAfter((&(tempsd->departure_time)),now)){//checking if it departs after now
        if (tempsd->cost < current_lowest_cost){//if we can get a better deal
          choice = tempsd;//updating our choice
          current_lowest_cost = tempsd->cost;//and our lowest cost flight found
        }
        else if (tempsd->cost == current_lowest_cost){//if the cost are the same then check with the arrival time
          if (isAfter((&(choice->arrival_time)),(&(tempsd->arrival_time)))){
            choice = tempsd;
          }
        }
      }
    }
    tempsd = tempsd->next;//traversing down the linked list of schedules
  }

  if(choice == NULL){//NULL is the initial value of choice, if it stayed this way means no choice had been found
    return false;
  }
  else {
    *departure = (choice->departure_time);//copying corresponding information of our choice
    *arrival = (choice->arrival_time);
    *cost = (current_lowest_cost);
    return true;
  }
}
