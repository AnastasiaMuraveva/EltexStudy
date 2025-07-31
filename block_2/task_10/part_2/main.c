#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    sigset_t set;
    int ret = 0;

    printf("PID: %d\n", getpid());

    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    ret = sigprocmask(SIG_BLOCK, &set, NULL);
    if (ret == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    while(1){
        continue;
    }

    exit(EXIT_SUCCESS);
}