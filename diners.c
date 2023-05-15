#include "diners.h"


/*  This function simulates the behavior of the philosopher that enters the room, eats, thinks, waits
    for a fork and exits. It is executed by every child forked when a new philosopher is spawned in the dining room.
    It initialize a counter that keeps track of the times the philosopher has eaten; It initialize 
    the personal semaphore and calls the functions grab_forks() and put_away_forks()*/
void philosopher();

/*  SIGINT handler
    prints the number of times that the exiting philosopher has eaten.*/
void sigTermHandler(){
	printf("Philosopher #%d has eaten %d times.\n", process_number, eat_count);
    logprint("A philosopher terminated\n");
	exit(0);
}

/*  SIGIUSR1 handler
    calls the enter() function and forks a new philosopher to join the dining room.*/
void sigProdHandler(){
    enter();
    process_number = last;
    phil_pid[process_number] = fork();
    if (phil_pid[process_number] == 0){ //child process
        philosopher();
    }else if (phil_pid[process_number] < 0){
        error("error: child fork failed\n");
    }
}
/*  SIGIUSR2 handler
    calls lthe eave() function and kills the philosopher that has been the first to enter.*/
void sigConsHandler(){
    leave();
    process_number = first-1;
    printf("Philosopher #%d pid: %d is leaving\n",process_number,phil_pid[process_number]);
    kill(phil_pid[process_number],SIGINT);
}


int main(int argc, char *argv[]) {
    signal(SIGINT, sigTermHandler);
    signal(SIGUSR1, sigProdHandler);
    signal(SIGUSR2, sigConsHandler);

    //open log file
    fp  = fopen ("data.log", "a+");
    if (fp == NULL) perror("log file couldn't open\n");

    // Create Shared memory
    int shmID, shmPID;   
    shmID = shmget(IPC_PRIVATE, MAX_PHIL_COUNT, IPC_CREAT | 0x1ff);
    philospher_states = (char*) shmat(shmID, NULL, 0);

    if ((long) shmID == -1 || (long) shmat == -1){
            printf("error: cannot allocate memory\n.");
            exit(1);
        }

    // Create Semaphores, last semaphore is mutex
    semset = semget(IPC_PRIVATE, MAX_PHIL_COUNT + 1, IPC_CREAT | 0x1ff);
    if (semset == -1) {
        printf("error: no more semaphores avaliable\n");
        exit(1);
    }
    sem_init(MAX_PHIL_COUNT,1); // Init mutex to 1



    // Initialize socket server
    int sockfd, newsockfd, portno, clilen, pid;
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    printf("waiting for instructions\n");
    
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while (1) {
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");
        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid == 0)  {
            close(sockfd);
            receiver(newsockfd);
            exit(0);
        }
        else close(newsockfd);
        /*int state;
        waitpid(pid,&state,0);    //waiting for child to return
        if (state == -1){      //Verify if child process is terminated without error
            error("\nThe child process terminated with an error!.\n"); 
        }*/
    } /* end of while */
    return 0; /* we never get here */
}   



// Child process
void philosopher(){
    eat_count=0;
    signal(SIGINT, sigTermHandler);

    // At the beginnging every philospher is waiting
  	philospher_states[SELF] = WAITING;
    phil_pid[SELF] = getpid();
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
