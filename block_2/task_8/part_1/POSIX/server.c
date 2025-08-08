#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define STR_SIZE 10
#define Q_NAME "/queue"


int main(){
    mqd_t qid;
    char message[STR_SIZE];
    int ret = 0;

    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = STR_SIZE,
        .mq_curmsgs = 0
    };

    qid = mq_open(Q_NAME, O_RDWR | O_CREAT | O_EXCL, 0644, &attr);
    if(qid == -1){
        perror("mq_open error");
        exit(EXIT_FAILURE);
    }

    strcpy(message, "Hello!");
    ret = mq_send(qid, message, strlen(message) + 1, 1);
    if(ret == -1){
        perror("mq_send error");
        mq_close(qid);
        mq_unlink(Q_NAME);
        exit(EXIT_FAILURE);
    }

    while(1){
        ret = mq_receive(qid, message, STR_SIZE, NULL);
        if(strcmp(message, "Hello!") == 0){
            ret = mq_send(qid, message, strlen(message) + 1, 1);
            continue;
        }
        break;
    }

    printf("Server received: %s\n", message);
    if(ret == -1){
        perror("mq_receive error");
        mq_close(qid);
        mq_unlink(Q_NAME);
        exit(EXIT_FAILURE);
    }

    mq_close(qid);
    mq_unlink(Q_NAME);
    exit(EXIT_SUCCESS);
}
