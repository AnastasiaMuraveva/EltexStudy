#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <err.h>

#define STR_LEN 10

int main(){
    char buf[STR_LEN] = { 0 };
    int fd_wr, fd_rd;
    int ret = 0;

    fd_rd = open("serv_wr", O_RDONLY);
    if(fd_rd == -1)
        err(EXIT_FAILURE, "Client first open error");

    fd_wr = open("client_wr", O_WRONLY);
    if(fd_wr == -1)
        err(EXIT_FAILURE, "Client second open error");

    ret = read(fd_rd, &buf, STR_LEN);
    if(ret == -1)
        err(EXIT_FAILURE, "Client read error");

    printf("Client read: %s\n", buf);

    strcpy(buf, "Hello!");
    
    ret = write(fd_wr, buf, strlen(buf) + 1);
    if(ret == -1)
        err(EXIT_FAILURE, "Client write error");

    exit(EXIT_SUCCESS);
}