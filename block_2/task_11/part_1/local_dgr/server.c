#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define PATH "/tmp/socket"
#define STR_LEN 10

int main(){
    struct sockaddr_un serv, client;
    int client_fd;
    char msg[STR_LEN];
    socklen_t client_size;
    int ret = 0;

    unlink(PATH);

    client_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if(client_fd == -1){
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, PATH, sizeof(serv.sun_path) - 1);

    ret = bind(client_fd, (struct sockaddr *) &serv, sizeof(serv));
    if(ret == -1){
        perror("bind error");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    client_size = sizeof(client);
    ret = recvfrom(client_fd, msg, STR_LEN, 0, (struct sockaddr *) &client, &client_size);
    if(ret == -1){
        perror("recvfrom error");
        close(client_fd);
        unlink(PATH);
        exit(EXIT_FAILURE);
    }
    printf("Server receive: %s\n", msg);

    strcpy(msg, "Hello!");

    ret = sendto(client_fd, msg, STR_LEN, 0, (struct sockaddr *) &client, client_size);
    if(ret == -1){
        perror("sendto error");
        close(client_fd);
        unlink(PATH);
        exit(EXIT_FAILURE);
    }

    close(client_fd);
    unlink(PATH);

    exit(EXIT_SUCCESS);
}
