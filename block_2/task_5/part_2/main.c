#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    pid_t first_pid, second_pid;
    int status;

    first_pid = fork();
    if (first_pid == -1){
        perror("Process 0 first fork error");
        exit(EXIT_FAILURE);
    }
    if(first_pid == 0){
        first_pid = fork();
        if (first_pid == -1){
            perror("Process 1 first fork error");
            exit(EXIT_FAILURE);
        }
        if(first_pid == 0){
            printf("Process 3 pid: %d\n", getpid());
            printf("Process 3 ppid: %d\n", getppid());
            exit(EXIT_SUCCESS);
        }
        
        second_pid = fork();
        if (second_pid == -1){
            if(waitpid(first_pid, &status, 0) == -1){
                perror("Process 1 first waitpid error");
                exit(EXIT_FAILURE);
            }
            perror("Process 1 second fork error");
            exit(EXIT_FAILURE);
        }
        if(second_pid == 0){
            printf("Process 4 pid: %d\n", getpid());
            printf("Process 4 ppid: %d\n", getppid());
            exit(EXIT_SUCCESS);
        }
        
        printf("Process 1 pid: %d\n", getpid());
        printf("Process 1 ppid: %d\n", getppid());
        if(waitpid(first_pid, &status, 0) == -1){
            perror("Process 1 first waitpid error");
            exit(EXIT_FAILURE);
        }
        if (waitpid(second_pid, &status, 0) == -1){
            perror("Process 1 second waitpid error");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }

    second_pid = fork();
    if (second_pid == -1){
        if(waitpid(first_pid, &status, 0) == -1){
            perror("Process 0 first waitpid error");
            exit(EXIT_FAILURE);
        }
        perror("Process 0 second fork error");
        exit(EXIT_FAILURE);
    }
    if(second_pid == 0){
        first_pid = fork();
        if (first_pid == -1){
            perror("Process 2 first fork error");
            exit(EXIT_FAILURE);
        }
        if(first_pid == 0){
            printf("Process 5 pid: %d\n", getpid());
            printf("Process 5 ppid: %d\n", getppid());
            exit(EXIT_SUCCESS);
        }

        if(waitpid(first_pid, &status, 0) == -1){
            perror("Process 2 first waitpid error");
            exit(EXIT_FAILURE);
        }
        printf("Process 2 pid: %d\n", getpid());
        printf("Process 2 ppid: %d\n", getppid());
        exit(EXIT_SUCCESS);
    }

    printf("Process 0 pid: %d\n", getpid());
    printf("Process 0 ppid: %d\n", getppid());

    if(waitpid(first_pid, &status, 0) == -1){
        perror("Process 0 first waitpid error");
        exit(EXIT_FAILURE);
    }
    if (waitpid(second_pid, &status, 0) == -1){
        perror("Process 0 second waitpid error");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}