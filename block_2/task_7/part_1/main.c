#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#define STR_LEN 10

int main(){
    int pipefd[2];
    int pipefd2[2];
    int pid;
    int status;
    char buf[STR_LEN] = { 0 };
    int ret = 0;

    ret = pipe(pipefd);
    if(ret == -1){
        perror("First pipe error");
        exit(EXIT_FAILURE);
    }
    ret = pipe(pipefd2);
    if(ret == -1){
        perror("Second pipe error");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if(pid == -1){
        perror("Fork error");
        exit(EXIT_FAILURE);
    }
    if (pid == 0){
        ret = close(pipefd[1]);
        if(ret == -1){
            perror("Child first close error");
            exit(EXIT_FAILURE);
        }
        ret = close(pipefd2[0]);
        if(ret == -1){
            perror("Child second close error");
            exit(EXIT_FAILURE);
        }

        ret = read(pipefd[0], &buf, STR_LEN);
        if(ret == -1){
            perror("Child read error");
            exit(EXIT_FAILURE);
        }
        printf("Child read: %s\n", buf);

        strcpy(buf, "Hello!");
        ret = write(pipefd2[1], buf, strlen(buf) + 1);
        if(ret == -1){
            perror("Child write error");
            exit(EXIT_FAILURE);
        }
        ret = close(pipefd[0]);
        if(ret == -1){
            perror("Child third close error");
            exit(EXIT_FAILURE);
        }
        ret = close(pipefd2[1]);
        if(ret == -1){
            perror("Child fourth close error");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    else{
        ret = close(pipefd[0]);
        if(ret == -1){
            perror("Parent first close error");
            exit(EXIT_FAILURE);
        }
        ret = close(pipefd2[1]);
        if(ret == -1){
            perror("Parent second close error");
            exit(EXIT_FAILURE);
        }
        strcpy(buf, "Hi!");
        ret = write(pipefd[1], buf, strlen(buf) + 1);
        if(ret == -1){
            perror("Parent write error");
            exit(EXIT_FAILURE);
        }

        ret = read(pipefd2[0], &buf, STR_LEN);
        if(ret == -1){
            perror("Parent read error");
            exit(EXIT_FAILURE);
        }
        printf("Parent read: %s\n", buf);
        ret = wait(&status);
        if(ret == -1){
            perror("Wait error");
            exit(EXIT_FAILURE);
        }
        ret = close(pipefd[1]);
        if(ret == -1){
            perror("Parent third close error");
            exit(EXIT_FAILURE);
        }
        ret = close(pipefd2[0]);
        if(ret == -1){
            perror("Parent fourth close error");
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}