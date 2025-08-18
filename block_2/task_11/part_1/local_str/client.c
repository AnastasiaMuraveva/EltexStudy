#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define PATH "/tmp/socket"
#define STR_LEN 10

int main(){
    struct sockaddr_un serv;
    int fd;
    char msg[STR_LEN];
    int ret = 0;

    fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(fd == -1){
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, PATH, sizeof(serv.sun_path) - 1);

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
