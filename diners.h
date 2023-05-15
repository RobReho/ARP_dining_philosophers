/*--------------Diners header-----------------
Included in the "diners.c" file, groups the functions dedicated to the handling 
of the dining philosophers server.
*/
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "fifo_queue.h"

// Constants
#define MAX_EAT_TIME 3  //seconds
#define THINK_TIME 3    //seconds
// States
#define THINKING 0
#define HUNGRY 1
#define EATING 2

// For easier usage
#define SELF process_number
#define LEFT (process_number==0) ? MAX_PHIL_COUNT : process_number-1
#define RIGHT (process_number+1)%MAX_PHIL_COUNT

FILE *fp;
volatile int eat_count;
char* philospher_states;
int process_number;
int semset;
pid_t phil_pid[MAX_PHIL_COUNT];
pid_t ppid;
int fd_p[2], fd_c[2];  //unnamed pipe file descriptors

/* Log file */
void logprint(char msg[]){
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    fprintf(fp, "%li : %li :: diners (%i): %s",current_time.tv_sec,current_time.tv_usec,getpid(),msg); 
    fflush(stdout);
}

/* Error handling function */
void error(char *msg){
    perror(msg);
    logprint(msg);
    exit(EXIT_FAILURE);
}

// Functions
/*  Returns a random amount of seconds that the philosopher will spend eating.
    The maximum amount of time is expressed by the defined variable MAX_EAT_TIME.*/
int random_time();

/*  SIGINT handler
    prints the number of times that the exiting philosopher has eaten. */
void sigTermHandler();

/*  Called by the function grab_forks(), if the state of the philosopher is HUNGRY and the philosophers 
    at the left and right side are not eating, he can grab the forks and start eating.
    Input argument: int process number of the philosopher.*/
void test_and_eat(int p);

/*  his function simulates the philosopher picking up the forks. It waits to take the mutex and calls the 
    function test_and_eat() to test if the forks at their sides are available, than releases the mutex.*/
void grab_forks();

/*  This function simulates the philosopher releasing the forks. It waits to take the mutex and switches the 
    state to THINKING. Then calls the function test_and_eat() for the philosophers at their side in order to
    give them the change to eat in case they are hungry.*/
void put_away_forks();

/* Init a semaphore -> INIT */
void sem_init(int semnr, int value);

/* Signal a semaphore to continue -> UP */
void sem_signal(int semnr);

/* Wait for a semaphore -> DOWN */
void sem_wait(int semnr);

/*  This function simulates the behavior of the philosopher that enters the room, eats, thinks, waits
    for a fork and exits. It is executed by every child forked when a new philosopher is spawned in the dining room.
    It initialize a counter that keeps track of the times the philosopher has eaten; It initialize 
    the personal semaphore and calls the functions grab_forks() and put_away_forks()*/
void philosopher();

/*  The parent function waits for the SIGUSR1 to be sent by the producer or consumer to check for a ready 
    pipe file descriptor through the primitive select(). When one or more fd are ready the function reads from it 
    calling the function read_from_pipe(int fd); Eventually, the parent function waits for the children process 
    producer and consumer to return. */
void parent();

/*  This function reads from the ready pipe and behaves differently according to
    the command received:
    -   if the message is "1", the function enter() will be called to update the philosophers'
        array and a child will be forked to execute the philosopher's function;
    -   if the message is "0", the function leave() will be called to update the philosophers'
        array and the appropriate child process willbe killed according to the queue.    */
int read_from_pipe(int fd);

/*  Returns a random fd among the two given as imput.
    used after the select function in the case that both the pipes' file descriptors
    are ready. */
int rand_fd(int fd1, int fd2);




int rand_fd(int fd1, int fd2)
{
    int rnd =(rand() %(2))+1;   //return random value between 1 and 2
    if(rnd==1){ 
        return fd1;
    }else if(rnd==2){
        return fd2;
    }
}

int read_from_pipe(int fd){
    logprint("reading_____________\n");   
    char req[16];
    usleep(100000);
    if(read(fd, &req, 80)==-1){    //read answer from FIFO
        close(fd);
        error("main - Failed to read from pipe\n");
    }

    switch (atoi(req)){
    
        case 1:
            logprint("phil entering\n");
            enter();
            //display();
            process_number = last;
            phil_pid[process_number] = fork();
            if (phil_pid[process_number] == 0){ //child process
                philosopher();
            }else if (phil_pid[process_number] < 0){
                error("error: child fork failed\n");
            } else 
                for(int i=0;i<MAX_PHIL_COUNT;i++){
                    printf("Pid %d: %d\n",i,phil_pid[i]); fflush(stdout);
                }
            break;

        case 0:
            logprint("phil exiting\n");
            leave();
            //display();
            process_number = first-1;
            printf("Philosopher #%d pid: %d is leaving\n",process_number,phil_pid[process_number]);
            kill(phil_pid[process_number],SIGINT);
            break;

        default:
            printf("Wrong message received\n");
    }
            
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
	philospher_states[SELF]=THINKING;
	test_and_eat(LEFT);
	test_and_eat(RIGHT);
  sem_signal(MAX_PHIL_COUNT); // Up mutex
}
