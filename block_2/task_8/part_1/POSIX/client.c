#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define STR_SIZE 10
#define Q_NAME "/queue"

int main(){
    mqd_t qid;
    char message[STR_SIZE];
    int ret = 0;

    while(1){
        qid = mq_open(Q_NAME, O_RDWR);
        if(qid == -1){
            if(errno == ENOENT)
                continue;
            perror("mq_open 1 error");
            exit(EXIT_FAILURE);
        }
        break;
    }

    ret = mq_receive(qid, message, STR_SIZE, NULL);
    if(ret == -1){
        perror("mq_receive error");
        mq_close(qid);
        exit(EXIT_FAILURE);
    }

    printf("Client received: %s\n", message);

    strcpy(message, "Hi!");
    ret = mq_send(qid, message, strlen(message) + 1, 1);
    if(ret == -1){
        perror("mq_send error");
        mq_close(qid);
        exit(EXIT_FAILURE);
    }

    mq_close(qid);
    exit(EXIT_SUCCESS);
}