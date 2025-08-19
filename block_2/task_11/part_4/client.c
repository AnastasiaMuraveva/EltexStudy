#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <signal.h>
#include <arpa/inet.h>

#define BUF_SIZE 2048

int flag = 1;
void sig_handler(int num, siginfo_t *info, void *args){
    printf("\nReceived signal #%d from %d\n", num, info->si_pid);
    flag = 0;
}

int main(){
    int fd;
    int ret = 0;
    unsigned short *data;
    char buf[BUF_SIZE];
    struct sockaddr_in serv;
    socklen_t len = sizeof(serv);
    struct sigaction act;
    sigset_t set;

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sig_handler;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    act.sa_mask = set;
    ret = sigaction(SIGINT, &act, NULL);
    if (ret < 0) {
        err(EXIT_FAILURE, "sigaction error");
    }

    fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(fd < 0){
        err(EXIT_FAILURE, "socket error");
    }

    while(flag){
        memset(buf, 0, BUF_SIZE);
        ret = recvfrom(fd, buf, BUF_SIZE, 0, (struct sockaddr *) &serv, &len);
        if(ret < 0){
            close(fd);
            err(EXIT_FAILURE, "recvfrom error");
        }
        data = buf;
        // Пропускаем IP-заголовок
        data+= 10;
        // Выводим UDP-заголовок
        printf("Source port: %hu\n", ntohs(*data));
        data++;
        printf("Destination port: %hu\n", ntohs(*data));
        data++;
        printf("Length: %hu\n", ntohs(*data));
        data++;
        printf("Check sum: %hu\n\n", ntohs(*data));
    }
    close(fd);
    exit(EXIT_SUCCESS);
}