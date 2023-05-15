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
    fprintf(fp, "%li : %li :: consumer (%i): %s",current_time.tv_sec,current_time.tv_usec,getpid(),msg); 
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

    printf("CONSUMER PROCESS\n");

    while(1){
        printf("Enter '0' make a Philosopher exit the room\n");
        scanf("%d", &ch);
        if(ch ==0){
            printf("Requesting a Philosopher to leave the table...\n");

            if(write(fdw, "0", sizeof(ch))==-1){
                error("sub - failed to write on FIFO\n");
                close(fdw);
            }
            kill(ppid,SIGUSR1);     //signal parent process
        }else
            printf("Wrong input: \nTo add a Philosopher, access the 'Producer' shell\n\n");
    }
    return 0;
}