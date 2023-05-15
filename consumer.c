#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/time.h>

FILE *fp;   //log file

/* Log file */
void logprint(char msg[]){
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    fprintf(fp, "%li : %li :: pid(%i): %s",current_time.tv_sec,current_time.tv_usec,getpid(),msg); 
    fflush(stdout);
}

/* Error handling function */
void error(char *msg){
    perror(msg);
    logprint(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    printf("CONSUMER PROCESS\n");

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting\n");

     while(1){
        printf("Enter 'x' to make a Philosopher leave the room\n");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        if(buffer[0] == 'x'){
            printf("Requesting a Philosopher to leave the table...\n");

            n = write(sockfd,buffer,strlen(buffer));
            if (n < 0) 
                error("ERROR writing to socket\n");

            bzero(buffer,256);
            n = read(sockfd,buffer,255);
            if (n < 0) 
                error("ERROR reading from socket\n");
            printf("%s\n",buffer);

        }else
            printf("Error: Enter 'x' to remove a Philosopher from the table\nTo add a Philosopher, access the 'Producer' shell\n");
     }
    return 0;
}