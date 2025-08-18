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
    int fd;
    char msg[MSG_SIZE];
    int ret = 0;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        err(EXIT_FAILURE, "socket error");
    }

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv.sin_port = htons(5050);

    ret = connect(fd, (struct sockaddr *) &serv, sizeof(serv));
    if(ret == -1){
        close(fd);
        err(EXIT_FAILURE, "connect error");
    }
    ret = recv(fd, msg, MSG_SIZE, 0);
    if(ret == -1){
        close(fd);
        err(EXIT_FAILURE, "recv error");
    }
    printf("TCP client receive: %s\n", msg);
    close(fd);
    exit(EXIT_SUCCESS);
}