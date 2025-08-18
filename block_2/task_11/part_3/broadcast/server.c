#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <err.h>
#define STR_LEN 10


int main(){
    int fd;
    int ret;
    int flag = 1;
    char msg[STR_LEN];
    time_t mytime;
    struct tm *now;
    struct sockaddr_in endpoint;

    endpoint.sin_addr.s_addr = INADDR_BROADCAST;
    endpoint.sin_port = htons(5050);
    endpoint.sin_family = AF_INET;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0){
        err(EXIT_FAILURE, "socket error");
    }
    ret = setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag));
    if(ret < 0){
        close(fd);
        err(EXIT_FAILURE, "setsockopt error");
    }

    mytime = time(NULL);
    now = localtime(&mytime);
    sprintf(msg, "%d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);

    ret = sendto(fd, msg, STR_LEN, 0, (struct sockaddr* ) &endpoint, sizeof(endpoint));
    if(ret < 0){
        close(fd);
        err(EXIT_FAILURE, "sendto error");
    }

    printf("end of server's work\n");
    close(fd);
    exit(EXIT_SUCCESS);
}