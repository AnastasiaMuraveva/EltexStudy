#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sig_handler(int num, siginfo_t *info, void *args){
    printf("Received signal #%d from %d\n", num, info->si_pid);
}

int main(){
    struct sigaction act;
    sigset_t set;
    int ret = 0;

    printf("PID: %d\n", getpid());

    act.sa_sigaction = sig_handler;

    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    act.sa_mask = set;

    ret = sigaction(SIGUSR1, &act, NULL);
    if (ret == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    while(1){
        continue;
    }

    exit(EXIT_SUCCESS);
}