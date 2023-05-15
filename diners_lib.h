#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// Constants
#define MAX_PHIL_COUNT 5
#define MAX_EAT_TIME 3
#define THINK_TIME 3
// States
#define WAITING 0
#define HUNGRY 1
#define EATING 2

// For easier usage
#define SELF process_number
#define LEFT (process_number==0) ? MAX_PHIL_COUNT : process_number-1
#define RIGHT (process_number+1)%MAX_PHIL_COUNT

// Functions
int random_time();
void sigTermHandler();
void test_and_eat(int p);
void grab_forks();
void put_away_forks();

volatile int eat_count;
char* philospher_states;
int process_number;


void sem_init(int semnr, int value);
void sem_signal(int semnr);
void sem_wait(int semnr);
void child();
void parent();


int semset;
//int process_number;

// SIGINT handler
void sigTermHandler(){
	printf("Philosopher #%d has eaten %d times.\n", process_number, eat_count);
	exit(0);
}

/* ----- SEMAPHORE HELPER FUNCTIONS ----- */

// Init a semaphore -> INIT
void sem_init(int semnr, int value) {
    int erg;
    erg = semctl(semset, semnr, SETVAL, value);
}

// Signal a semaphore to continue -> UP
void sem_signal(int semnr) {
    struct sembuf semops;
    int erg;
    semops.sem_num = semnr;
    semops.sem_op = 1; // Up the semaphore
    semops.sem_flg = 0;

    erg = semop(semset, &semops, 1);
}

// Wait for a semaphore -> DOWN
void sem_wait(int semnr) {
    struct sembuf semops;
    int erg;
    semops.sem_num = semnr;
    semops.sem_op = -1; // Down the semaphore
    semops.sem_flg = 0;
    erg = semop(semset, &semops, 1);
}

/* ----- Dining Philosophers Problem -----*/

// Generate random eating time
int random_time(){
    int time;
    time=(MAX_EAT_TIME+rand()%(MAX_EAT_TIME-1));
    return time;
}

// Test whether a philosopher can eat
void test_and_eat(int p){
	if (philospher_states[p] == HUNGRY 
        && philospher_states[LEFT] != EATING 
        && philospher_states[RIGHT] != EATING){
		    philospher_states[p]=EATING;
            sem_signal(p);
	}
}

// One grabs fork
void grab_forks(){
  sem_wait(MAX_PHIL_COUNT); // Down mutex
	printf("Philosopher #%d is hungry.\n", process_number);
	philospher_states[SELF]=HUNGRY;
	test_and_eat(SELF);
  sem_signal(MAX_PHIL_COUNT);
  sem_wait(SELF);
}

// Put away forks
void put_away_forks(){
  sem_wait(MAX_PHIL_COUNT); // Down mutex
	philospher_states[SELF]=WAITING;
	test_and_eat(LEFT);
	test_and_eat(RIGHT);
  sem_signal(MAX_PHIL_COUNT); // Up mutex
}

// Child process
void child(){
    eat_count=0;
        signal(SIGINT, sigTermHandler);

    // At the beginnging every philospher is waiting
  	philospher_states[SELF]=WAITING;
    sem_init(process_number,0);  //initialize semaphore 
    
    printf("Philosopher %d joined the table.\n", process_number);

    while(1){
        printf("Philosopher #%d is waiting.\n", process_number);
        sleep(THINK_TIME);
        grab_forks();
        printf("Philosopher #%d is eating.\n", process_number);
        eat_count++;
        sleep(random_time()); // Eat.
        put_away_forks();
    }
}

// Parent process, wait until all children are finished
void parent(){
    printf("parent waiting for instructions\n");fflush(stdout);
     
            //wait for socket message
            int req;
            scanf("%d",&req);
            
     
  wait(NULL);
}








