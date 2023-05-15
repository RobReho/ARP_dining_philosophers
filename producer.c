#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/time.h>

FILE *fp;

/* Log file */
void logprint(char msg[]){
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    fprintf(fp, "%li : %li :: producer (%i): %s",current_time.tv_sec,current_time.tv_usec,getpid(),msg); 
    fflush(stdout);
}

/* Error handling function */
void error(char *msg){
    perror(msg);
    logprint(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int ch;
    int fdw = atoi(argv[1]);
    int ppid = atoi(argv[2]);


    printf("PRODUCER PROCESS\n");

    while(1){
        printf("Enter '1' to make a Philosopher exit the room\n");
        scanf("%d", &ch);
        if(ch == 1){
            printf("Requesting a Philosopher to join the table...\n");

            if(write(fdw, "1", sizeof(ch))==-1){
                close(fdw);
                error("failed to write on FIFO\n");
            }
            kill(ppid,SIGUSR1); //signal parent
        }else
            printf("Wrong input: \nTo call back a Philosopher, access the 'Consumer' shell\n\n");
    }
    return 0;
}