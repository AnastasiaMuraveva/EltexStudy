#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    sigset_t set;
    int ret = 0;
    int sig_num;

    printf("PID: %d\n", getpid());

    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);

    ret = sigprocmask(SIG_BLOCK, &set, NULL);
    if (ret == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    while(1){
        sigwait(&set, &sig_num);
        printf("Received signal #%d\n", sig_num);
    }

    exit(EXIT_SUCCESS);
}