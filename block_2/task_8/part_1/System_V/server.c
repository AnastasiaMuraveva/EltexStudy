#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/msg.h>
#include <stdlib.h>
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
    qid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if(qid == -1){
        perror("msgget error");
        exit(EXIT_FAILURE);
    }

    message.mtype = 1;
    strcpy(message.mtext, "Hello!");
    ret = msgsnd(qid, &message, strlen(message.mtext) + 1, 0);
    if(ret == -1){
        perror("msgsnd error");
        msgctl(qid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }

    ret = msgrcv(qid, &message, sizeof(message), 2, 0);
    if(ret == -1){
        perror("msgrcv error");
        msgctl(qid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }
    printf("Server received: %s\n", message.mtext);

    ret = msgctl(qid, IPC_RMID, NULL);
    if(ret == -1){
        perror("msgctl error");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}