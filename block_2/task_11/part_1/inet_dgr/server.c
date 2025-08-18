#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define STR_LEN 10

int main(){
    struct sockaddr_in serv, client;
    int client_fd;
    char msg[STR_LEN];
    socklen_t client_size;
    int ret = 0;

    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(client_fd == -1){
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY; 
    serv.sin_port = htons(5050);

    ret = bind(client_fd, (struct sockaddr *) &serv, sizeof(serv));
    if(ret == -1){
        perror("bind error");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    client_size = sizeof(client);
    ret = recvfrom(client_fd, msg, STR_LEN, 0, (struct sockaddr *) &client, &client_size);
    if(ret == -1){
        perror("recv error");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server receive: %s\n", msg);

    strcpy(msg, "Hello!");
    ret = sendto(client_fd, msg, STR_LEN, 0, (struct sockaddr *) &client, client_size);
    if(ret == -1){
        perror("send error");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    close(client_fd);
    exit(EXIT_SUCCESS);
}
