/*--------------FIFO queue header-----------------
Included in the "diners.h" header, groups the functions dedicated to the handling 
of the fifo queue of the philosophers entering and exiting the dining room.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PHIL_COUNT 5    //Maximum number of philosophers at the table

int queue[MAX_PHIL_COUNT];
int first = -1;
int last = -1;

/*  Function that adds a philosopher at the table: 
    increases the occupied seats by writing a "1" into 
    an array of lenght 5 (number of max philosophers) until 
    the array is full.  */
void enter();

/*  Function that remove a philosopher from the table: 
    decreases the occupied seats by moving foward the first
    element of the array until the last spot.  */
void leave();

/*  Function that prints the pid of the processes (philosophers)
    that have entered the room.  */
//void display();

void enter(){
    int phil = 1;
    if(last == MAX_PHIL_COUNT - 1)
    printf("Table is full\n");
    else{
        if(first == -1)
        first = 0;
        printf("Occupying one spot at the table\n");
        last++;
        queue[last] = phil;
    }
}

void leave(){
    if(first == -1 || first > last){
        printf("Table empty\n");
        first = -1;
        last = -1;
        return;
    }
    else{
        first++;
    }
}
/*        
void display(){
    int i;
    if(first == -1)
        logprint("Table empty\n");
    else{
        logprint("Philosophers in the room are: ");
        for(i = first; i <= last; i++){
            printf("%i ",i);    //queue[i] to print the content of the array
        }
        printf("\n");
    }
        printf("first: %d\nlast: %d\n",first, last);

}*/