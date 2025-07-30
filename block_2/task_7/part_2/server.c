#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <err.h>
#include <errno.h>

#define STR_LEN 10

int main(){
    char buf[STR_LEN] = { 0 };
    int fd_wr, fd_rd;
    int ret = 0;

    ret = mkfifo("serv_wr", 0644);
    if(ret == -1 && errno != EEXIST)
        err(EXIT_FAILURE, "First mkfifo error");

    ret = mkfifo("client_wr", 0644);
    if(ret == -1 && errno != EEXIST)
        err(EXIT_FAILURE, "Second mkfifo error");

    fd_wr = open("serv_wr", O_WRONLY);
    if(fd_wr == -1)
        err(EXIT_FAILURE, "First server open error");

    fd_rd = open("client_wr", O_RDONLY);
    if(fd_rd == -1)
        err(EXIT_FAILURE, "Second server open error");
    
    strcpy(buf, "Hi!");

    ret = write(fd_wr, buf, strlen(buf) + 1);
    if(ret == -1)
        err(EXIT_FAILURE, "Server write error");

    ret = read(fd_rd, &buf, STR_LEN);
    if(ret == -1)
        err(EXIT_FAILURE, "Server read error");

    printf("Server read: %s\n", buf);

    ret = close(fd_wr);
    if(ret == -1)
        err(EXIT_FAILURE, "First server close error");

    ret = close(fd_rd);
    if(ret == -1)
        err(EXIT_FAILURE, "Second server close error");

    exit(EXIT_SUCCESS);
}