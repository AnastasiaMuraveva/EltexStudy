#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <arpa/inet.h>

#define STR_LEN 8

struct frame{
    uint16_t source_port;
    uint16_t dest_port;
    uint16_t len;
    uint16_t checksum;
    char payload[STR_LEN];
};

int main(){
    struct sockaddr_in serv;
    socklen_t sin_len = sizeof(serv);
    int fd;
    struct frame udp_frame;
    int msg_len = sizeof(udp_frame) + 20;
    char msg[msg_len];
    unsigned short *data;
    int ret = 0;

    udp_frame.source_port = htons(7777);
    udp_frame.dest_port = htons(5050);
    udp_frame.len = htons(sizeof(udp_frame));
    udp_frame.checksum = 0;
    strcpy(udp_frame.payload, "Hi");

    fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(fd == -1){
        err(EXIT_FAILURE, "socket error");
    }

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv.sin_port = udp_frame.dest_port;

    ret = sendto(fd, &udp_frame, sizeof(udp_frame), 0, (struct sockaddr *) &serv, sizeof(serv));
    if(ret == -1){
        close(fd);
        err(EXIT_FAILURE, "sendto error");
    }

    while(1){
        memset(msg, 0, msg_len);
        ret = recvfrom(fd, msg, msg_len, 0, (struct sockaddr *) &serv, &sin_len);
        if(ret == -1){
            close(fd);
            err(EXIT_FAILURE, "recvfrom error");
        }
        data = msg;
        data += 11;
        if(ntohs(*data) == 7777)
            break;
    }

    printf("Client receive: %s\n", &msg[msg_len - STR_LEN]);

    close(fd);
    exit(EXIT_SUCCESS);
}
