#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>

#define STR_LEN 8

int main(){
    struct sockaddr_in serv, client;
    socklen_t client_size = sizeof(client);
    int client_fd;
    char msg[STR_LEN];
    int ret = 0;

    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(client_fd == -1){
        err(EXIT_FAILURE, "socket error");
    }

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY; 
    serv.sin_port = htons(5050);

    ret = bind(client_fd, (struct sockaddr *) &serv, sizeof(serv));
    if(ret == -1){
        close(client_fd);
        err(EXIT_FAILURE, "bind error");
    }

    ret = recvfrom(client_fd, msg, STR_LEN, 0, (struct sockaddr *) &client, &client_size);
    if(ret == -1){
        close(client_fd);
        err(EXIT_FAILURE, "recvfrom error");
    }
    printf("Server receive: %s\n", msg);

    strcat(msg, "!");
    ret = sendto(client_fd, msg, STR_LEN, 0, (struct sockaddr *) &client, client_size);
    if(ret == -1){
        close(client_fd);
        err(EXIT_FAILURE, "sendto error");
    }

    close(client_fd);
    exit(EXIT_SUCCESS);
}
