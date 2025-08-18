#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define STR_LEN 10

int main(){
    struct sockaddr_in serv;
    int fd;
    char msg[STR_LEN];
    int ret = 0;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv.sin_port = htons(5050);

    ret = connect(fd, (struct sockaddr *) &serv, sizeof(serv));
    if(ret == -1){
        perror("connect error");
        close(fd);
        exit(EXIT_FAILURE);
    }

    ret = recv(fd, msg, STR_LEN, 0);
    if(ret == -1){
        perror("recv error");
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("Client receive: %s\n", msg);

    strcpy(msg, "Hi!");
    ret = send(fd, msg, STR_LEN, 0);
    if(ret == -1){
        perror("send error");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
    exit(EXIT_SUCCESS);
}