#include "diners.h"
#include <stdbool.h>

pid_t cons, prod;

// SIGINT handler
void sigTermHandler(){
	printf("Philosopher #%d has eaten %d times.\n", process_number, eat_count);
    philospher_states[SELF]=THINKING;
	exit(0);
}

bool reading = false;

// SIGUSR1 handler
void sigReadHandler(){
	printf("READ\n");
    reading = true;
}



// _____________________________________________________________________________________________________________________
int main(int argc, char *argv[]) {
    signal(SIGINT, sigTermHandler);

    //open log file
    fp  = fopen ("data.log", "a+");
    if (fp == NULL) perror("log file couldn't open\n");

    char  fdw_p[2], fdw_c[2], chppid[5];
    ppid = getpid();
    sprintf(chppid,"%d",ppid);      

    // Create Shared memory to store the philosophers state
    int shmID;   
    shmID = shmget(IPC_PRIVATE, MAX_PHIL_COUNT, IPC_CREAT | 0x1ff);
    philospher_states = (char*) shmat(shmID, NULL, 0);

    if ((long) shmID == -1 || (long) shmat == -1){
		error("error: cannot allocate memory\n.");
	}

    // Create Semaphores, last semaphore is a mutex
    semset = semget(IPC_PRIVATE, MAX_PHIL_COUNT + 1, IPC_CREAT | 0x1ff);
    if (semset == -1) {
        error("error: no more semaphores avaliable\n");
    }
    sem_init(MAX_PHIL_COUNT,1); // Init mutex to 1

    // Create unnamed pipes
    if(pipe(fd_p)==-1){
        error("main: producer pipe opening failed\n");
    }
    if(pipe(fd_c)==-1){
        error("main: consumer pipe opening failed\n");
    }


// Fork Consumer process
    cons = fork();
    if (cons < 0){    //error on function fork
        error("consumer fork failed\n");
    }
    else if (cons == 0){ //child process: execute consumer
        close(fd_p[0]); //close unused pipes file descriptors
        close(fd_p[1]);
        close(fd_c[0]);
    
        sprintf(fdw_c,"%d",fd_c[1]);    
        char *argv_c[]={"/usr/bin/konsole","-e","./con",fdw_c,chppid,(char*)NULL};

        if(execvp(argv_c[0], argv_c) == -1){
            error("main - Exec_Error: exec consumer\n");
        }

    }else{  //parent
// Fork Producer process
        prod = fork();
        if (prod < 0){    //error on function fork
            error("producer fork failed\n");
        }
        else if (prod == 0){ //child process: execute producer
            close(fd_c[0]); //close unused pipes file descriptors
            close(fd_c[1]);
            close(fd_p[0]);
            
            sprintf(fdw_p,"%d",fd_p[1]);    
            char *argv_p[]={"/usr/bin/konsole","-e","./pro",fdw_p,chppid,(char*)NULL};

            if(execvp(argv_p[0], argv_p) == -1){
                error("main - Exec_Error: exec producer\n");
                }
        }else{  //____________________________________________parent___________________________
            parent();
        }
    }
  return 0;
}


// Child process
void philosopher(){
    eat_count=0;
    signal(SIGINT, sigTermHandler);

    // At the beginnging every philospher is thinking
  	philospher_states[SELF]=THINKING;
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

// Parent process, waits until all children return
void parent(){
   
    signal(SIGUSR1, sigReadHandler);
    fd_set ready_fds, current_fds;
    struct timeval tv;
    int ready_desc;
    struct timeval result;
    close(fd_c[1]); //close unused pipes file descriptors
    close(fd_p[1]);
    FD_ZERO(&current_fds);
    FD_SET(fd_p[0], &current_fds);
    FD_SET(fd_c[0], &current_fds);

    printf("waiting for instructions\n");

    while(1){
        //wait for pipe message
        if(reading){
            ready_fds = current_fds;    //select is destructive;
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            ready_desc = select(FD_SETSIZE, &ready_fds, NULL, NULL, &tv);
                
            switch (ready_desc){
                case 0:             //none of the fd is ready
                    if (ready_desc == -1){
                        error("select failed\n");
                    }
                    reading = false;
                break;

                case 1:             //one of the fd is ready
                    if(FD_ISSET(fd_p[0],&ready_fds)){

                        read_from_pipe(fd_p[0]);
                    
                    }
                    else if(FD_ISSET(fd_c[0],&ready_fds)){

                        read_from_pipe(fd_c[0]);

                    }
                    reading = false;
                break;

                case 2:          //both fd are ready
                    if(FD_ISSET(fd_p[0],&ready_fds) && FD_ISSET(fd_c[0],&ready_fds)){

                        int fd = rand_fd(fd_p[0],fd_c[0]);
                        
                        read_from_pipe(fd);
                    }
                    reading = false;
                break;
            }
        }
    }
    
    int state1, state2;
    waitpid(cons,&state1,0);    //waiting for children to return
    waitpid(prod,&state2,0);
	if (state1||state2 == -1){      //Verify if child process is terminated without error
		error("\nThe child process terminated with an error!.\n"); 
	    }
}
