#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <errno.h>
#define STR_SIZE 10

struct msgbuf {
    long mtype; 
    char mtext[STR_SIZE];

}; 

int main(){
    key_t key;
    int qid;
    struct msgbuf message;
    char *path = "path.txt";
    int id = 111;
    int ret = 0;

    key = ftok(path, id);
    if(key == -1){
        perror("ftok error");
        exit(EXIT_FAILURE);
    }
    while(1){
        qid = msgget(key, 0);
        if(qid == -1){
            if(errno == ENOENT)
                continue;
            perror("msgget error");
            exit(EXIT_FAILURE);
        }
        break;
    }

    ret = msgrcv(qid, &message, STR_SIZE, 1, 0);
    if(ret == -1){
        perror("msgrcv error");
        exit(EXIT_FAILURE);
    }
    printf("Client received: %s\n", message.mtext);

    message.mtype = 2;
    strcpy(message.mtext, "Hi!");
    ret = msgsnd(qid, &message, sizeof(message), 0);
    if(ret == -1){
        perror("msgsnd error");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}