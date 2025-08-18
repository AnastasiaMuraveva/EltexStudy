#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <err.h>
#include <arpa/inet.h>

#define STR_LEN 10

int main(){
    int fd;
    struct sockaddr_in client_addr, serv_addr;
    socklen_t size = sizeof(serv_addr);
    char msg[STR_LEN];
    int ret;
    struct ip_mreqn mreqn;

    mreqn.imr_multiaddr.s_addr = inet_addr("224.0.0.1");
    mreqn.imr_address.s_addr = INADDR_ANY;
    mreqn.imr_ifindex = 0;


    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(5050);
    client_addr.sin_family = AF_INET;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0){
        err(EXIT_FAILURE, "socket error");
    }
    ret = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreqn, sizeof(mreqn));
    if(ret < 0){
        close(fd);
        err(EXIT_FAILURE, "setsockopt error");
    }

    ret = bind(fd, (struct sockaddr* ) &client_addr, sizeof(client_addr));
    if(ret < 0){
        close(fd);
        err(EXIT_FAILURE, "bind error");
    }

    ret = recvfrom(fd, msg, STR_LEN, 0, (struct sockaddr* ) &serv_addr, &size);
    if(ret < 0){
        close(fd);
        err(EXIT_FAILURE, "recvfrom error");
    }
    printf("Client receive: %s\n", msg);

    close(fd);
    exit(EXIT_SUCCESS);
}