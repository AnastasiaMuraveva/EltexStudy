#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define PATH "/tmp/socket"
#define PATH_CL "/tmp/socket_client"
#define STR_LEN 10

int main(){
    struct sockaddr_un serv, client;
    int fd;
    char msg[STR_LEN];
    int ret = 0;

    fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if(fd == -1){
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    // Выполняем bind на клиенте, иначе сервер не будет знать куда отправлять сообщение
    client.sun_family = AF_LOCAL;
    strncpy(client.sun_path, PATH_CL, sizeof(client.sun_path) - 1);
    ret = bind(fd, (struct sockaddr *) &client, sizeof(client));
    if(ret == -1){
        perror("bind error");
        close(fd);
        unlink(PATH_CL);
        exit(EXIT_FAILURE);
    }

    serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, PATH, sizeof(serv.sun_path) - 1);

    ret = connect(fd, (struct sockaddr *) &serv, sizeof(serv));
    if(ret == -1){
        perror("connect error");
        close(fd);
        unlink(PATH_CL);
        exit(EXIT_FAILURE);
    }

    strcpy(msg, "Hi!");
    ret = send(fd, msg, STR_LEN, 0);
    if(ret == -1){
        perror("send error");
        close(fd);
        unlink(PATH_CL);
        exit(EXIT_FAILURE);
    }

    ret = recv(fd, msg, STR_LEN, 0);
    if(ret == -1){
        perror("recv error");
        close(fd);
        unlink(PATH_CL);
        exit(EXIT_FAILURE);
    }
    printf("Client receive: %s\n", msg);

    close(fd);
    unlink(PATH_CL);
    exit(EXIT_SUCCESS);
}
