#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/time.h>
#include "fifo_queue.h"
#include <sys/wait.h>

#define MAX_PHIL_COUNT 5
#define MAX_EAT_TIME 3  //seconds
#define THINK_TIME 3    //seconds
// States
#define WAITING 0
#define HUNGRY 1
#define EATING 2
//Table spot
#define SELF process_number
#define LEFT (process_number==0) ? MAX_PHIL_COUNT : process_number-1
#define RIGHT (process_number+1)%MAX_PHIL_COUNT

FILE *fp;   //log file
volatile int eat_count;
int process_number;
int fd;
char * myfifo = "/tmp/myfifo";
pid_t phil_pid[MAX_PHIL_COUNT];
char* philospher_states;
int semset;


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
    exit(1);
}

/*  Function executed by every child forked when a new socket connection is created.
    In a while loop it waits and reads the messages sent by the client with the passed socket 
    file descriptor. Depending on the client the message received will be either "1" or "x".
    The function recognise the message and call the appropriate functions by sending a signal 
    to the parent process.
    Input argument: int new_socket_fd  */
void receiver (int sock);

/*  Returns a random amount of seconds that the philosopher will spend eating.
    The maximum amount of time is expressed by the defined variable MAX_EAT_TIME.*/
int random_time();

/*  Called by the function grab_forks(), if the state of the philosopher is HUNGRY and the philosophers 
    at the left and right side are not eating, he can grab the forks and start eating.
    Input argument: int process number of the philosopher. */
void test_and_eat(int p);

/*  This function simulates the philosopher picking up the forks. It waits to take the mutex and calls the 
    function test_and_eat() to test if the forks at their sides are available, than releases the mutex. */
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



void receiver (int sock){
    int n;
    char buffer[256];
    printf("succesful connection\n");
    logprint("succesful connection\n");
    while(1){
        bzero(buffer,256);
        n = read(sock,buffer,255);
        if (n < 0) perror("ERROR reading from socket");
        printf("Message received: %s\n",buffer);
        logprint("received a message\n");

        n = write(sock,"I got your message",18);
        if (n < 0) error("ERROR writing to socket\n");

        switch (buffer[0]){
            case '1':
                logprint("Received the request to add a philosopher\n");
                kill(getppid(),SIGUSR1);
                break;

            case 'x':
                logprint("Received the request to remove a philosopher\n");
                kill(getppid(),SIGUSR2);
                break;
        
            default:
                printf("Non valid message, please ask again\n");
        }
    }
}



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
