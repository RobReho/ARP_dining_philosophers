#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX 5

void enter();
void leave();
void display();

int queue[MAX];
int first = -1;
int last = -1;

void enter();
void exit();
void display();

int main(int argc, char *argv[]){
    
	printf("im here...\n");
    while(1){
    printf("waiting for instructions\n");
    int req;
    scanf("%d",&req);
    
        switch (req)
        {
            case 1:
                printf("phil entering\n");
                enter();
                display();
                break;
            case 0:
                printf("phil exiting\n");
                leave();
                display();
                break;
            default:
                printf("Wrong choice, ask again\n");
        }
    } 
    
    return 0;
}

void enter(){
    int phil = 1;
    if(last == MAX - 1)
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
        printf("Philosopher %i has left the room\n",last);
        first++;
    }
}
        
void display(){
    int i;
    if(first == -1)
        printf("Table empty\n");
    else{
        printf("Philosophers in the room are: ");
        for(i = first; i <= last; i++){
            printf("%i ",queue[i]);
        }
        printf("\n");
    }
        printf("first: %d\nlast: %d\n",first, last);

}