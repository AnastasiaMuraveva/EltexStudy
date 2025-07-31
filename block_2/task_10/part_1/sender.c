#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

int main(int argc, char* argv[]){
    int pid = 0;
    int sig = 0;
    int ret = 0;
    if(argc > 3){
        printf("Too many arguments\n");
        exit(EXIT_FAILURE);
    }
    else if(argc < 3){
        printf("Too few arguments\n");
        exit(EXIT_FAILURE);
    }
    sig = atoi(argv[1]);
    pid = atoi(argv[2]);
    ret = kill(pid, sig);
    if (ret == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}