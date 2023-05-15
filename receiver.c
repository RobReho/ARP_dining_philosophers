#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <strings.h>

int main(int argc, char *argv[]) {
    printf("connected\n");
    int sock = atoi(argv[0]);
    int n;
    char buffer[256];
    while(1){
            printf("connected_1\n");

        bzero(buffer,256);
        n = read(sock,buffer,255);
        if (n < 0) perror("ERROR reading from socket");
        printf("Here is the message: %s\n",buffer);
        printf("---dostu %d\n",getpid()); fflush(stdout);
            printf("connected_2\n");

        n = write(sock,"I got your message",18);
        if (n < 0) perror("ERROR writing to socket");
            printf("connected_3\n");

        switch (buffer[0]){
            case '1':
                        printf("connected_4\n");

                printf("call signal 1\n");
                printf("my pid %d - parent pid %d\n",getpid(),getppid());
                kill(getppid(),SIGUSR1);
                break;

            case 'x':
                        printf("connected_5\n");

                printf("call signal x\n");
                printf("my pid %d - parent pid %d\n",getpid(),getppid());
                kill(getppid(),SIGUSR2);
                break;

            default:
                printf("Non valid message, ask again\n");
        }
                    printf("connected_6\n");

    }
}