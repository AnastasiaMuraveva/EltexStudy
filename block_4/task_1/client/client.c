#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <arpa/inet.h>

#include "client_func.h"

int main(){
    struct sockaddr_in serv;
    socklen_t sin_len = sizeof(serv);
    int fd;
    struct frame udp_frame = { 0 };
    int msg_len = SERVER_MSG_LEN + HEADERS_SIZE;
    char msg[msg_len];
    unsigned short *ptr;
    int ret = 0;
    int port;

    // Для того чтобы несколько клиентов корректно подключались с одного устройства
    // выбираем порт самостоятельно
    while(1){
        printf("Input an port number: ");
        scanf("%d", &port);
        if(port < 1024 || port > 49151){
            printf("Wrong port value\n");
            continue;
        }
        break;
    }
    // Убираем символ новой строки из stdin
    getchar();

    // Заполняем заголовок
    udp_frame.source_port = htons(port);
    udp_frame.dest_port = htons(5050);
    udp_frame.len = htons(sizeof(udp_frame));
    udp_frame.checksum = 0;

    fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(fd == -1){
        err(EXIT_FAILURE, "socket error");
    }

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("192.168.1.113");
    serv.sin_port = udp_frame.dest_port;

    sprintf(udp_frame.payload, "%d", udp_frame.source_port);
    ret = sendto(fd, &udp_frame, sizeof(udp_frame), 0, (struct sockaddr *) &serv, sizeof(serv));
    if(ret == -1){
        close(fd);
        err(EXIT_FAILURE, "sendto 1 error");
    }

   while(1){
        memset(udp_frame.payload, 0, CLIENT_MSG_LEN);
        printf("Enter a message: ");
        ret = input_string(udp_frame.payload, CLIENT_MSG_LEN);
        if(ret == -1){
            close(fd);
            printf("input string error\n");
            exit(EXIT_FAILURE);
        }
        ret = sendto(fd, &udp_frame, sizeof(udp_frame), 0, (struct sockaddr *) &serv, sizeof(serv));
        if(ret == -1){
            close(fd);
            err(EXIT_FAILURE, "sendto 2 error");
        }
        if(strcmp(udp_frame.payload, "exit") == 0)
            break;
        while(1){
            ret = recvfrom(fd, msg, msg_len, 0, (struct sockaddr *) &serv, &sin_len);
            if(ret == -1){
                close(fd);
                err(EXIT_FAILURE, "recvfrom error");
            }
            ptr = (unsigned short *) msg;
            ptr += 11;
            if(ntohs(*ptr) == port)
                break;
        }
        if(strcmp(&msg[HEADERS_SIZE], "exit") == 0){
            printf("Server has shut down\n");
            break;
        }
        printf("Server's response: %s\n", &msg[HEADERS_SIZE]);
        memset(msg, 0, msg_len);
    }
    close(fd);
    exit(EXIT_SUCCESS);
}
