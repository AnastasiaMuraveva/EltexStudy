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
    int client_fd, listen_fd;
    char msg[STR_LEN];
    socklen_t client_size;
    int ret = 0;

    listen_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(listen_fd == -1){
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, PATH, sizeof(serv.sun_path) - 1);

    ret = bind(listen_fd, (struct sockaddr *) &serv, sizeof(serv));
    if(ret == -1){
        perror("bind error");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    ret = listen(listen_fd, 1);
    if(ret == -1){
        perror("listen error");
        close(listen_fd);
        unlink(PATH);
        exit(EXIT_FAILURE);
    }

    client_size = sizeof(client);
    client_fd = accept(listen_fd, (struct sockaddr *) &client, &client_size);
    if(client_fd == -1){
        perror("accept error");
        close(listen_fd);
        unlink(PATH);
        exit(EXIT_FAILURE);
    }

    strcpy(msg, "Hello!");
    ret = send(client_fd, msg, STR_LEN, 0);
    if(ret == -1){
        perror("send error");
        close(listen_fd);
        close(client_fd);
        unlink(PATH);
        exit(EXIT_FAILURE);
    }

    ret = recv(client_fd, msg, STR_LEN, 0);
    if(ret == -1){
        perror("recv error");
        close(listen_fd);
        close(client_fd);
        unlink(PATH);
        exit(EXIT_FAILURE);
    }

    printf("Server receive: %s\n", msg);

    close(client_fd);
    close(listen_fd);
    unlink(PATH);

    exit(EXIT_SUCCESS);
}
