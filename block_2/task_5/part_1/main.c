#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    pid_t child_pid;
    int status;

    child_pid = fork();
    if (child_pid == -1){
        perror("Fork error");
        exit(EXIT_FAILURE);
    }
    if(child_pid == 0){
        printf("Child pid: %d\n", getpid());
        printf("Child ppid: %d\n", getppid());
        exit(0);
    }
    else{
        printf("Parent pid: %d\n", getpid());
        printf("Parent ppid: %d\n", getppid());
        wait(&status);
        printf("\nStatus: %d\n", WEXITSTATUS(status));
    }
    exit(EXIT_SUCCESS);
}