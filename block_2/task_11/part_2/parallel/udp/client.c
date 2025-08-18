#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#define MSG_SIZE 10

int main(){
    struct sockaddr_in serv;
    socklen_t len = sizeof(serv);
    int fd;
    char msg[MSG_SIZE] = { 0 };
    int ret = 0;
    int port;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        err(EXIT_FAILURE, "socket error");
    }

    port = 5050;
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv.sin_port = htons(port);

    strcpy(msg, "!");
    ret = sendto(fd, msg, MSG_SIZE, 0, (struct sockaddr *) &serv, sizeof(serv));
    if(ret < 0){
        close(fd);
        err(EXIT_FAILURE, "sendto error");
    }

    ret = recvfrom(fd, msg, MSG_SIZE, 0, (struct sockaddr *) &serv, &len);
    if(ret < 0){
        close(fd);
        err(EXIT_FAILURE, "recvfrom 1 error");
    }

    port = atoi(msg);
    serv.sin_port = htons(port);

    printf("Server new port: %d\n", ntohs(serv.sin_port));

    ret = recvfrom(fd, msg, MSG_SIZE, 0, (struct sockaddr *) &serv, &len);
    if(ret < 0){
        close(fd);
        err(EXIT_FAILURE, "recvfrom 2 error");
    }

    printf("Client receive: %s\n", msg);
    close(fd);
    exit(EXIT_SUCCESS);
}