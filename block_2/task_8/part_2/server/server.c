#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "lists.h"
#define Q_SERV_RCV "/serv_rcv"

// Работа сервера зеавершается прерыванием с клавиатуры

void sig_handler(int num, siginfo_t *info, void *args){
    printf("\nReceived signal #%d from %d\n", num, info->si_pid);
}

int main(){
    mqd_t qid_rcv;
    char message[FULL_MSG_SIZE];
    int ret = 0;
    unsigned int prior = 0;
    struct users_list *uhead = NULL;
    struct users_list *utail = NULL;
    struct history_list *hhead = NULL;
    struct history_list *htail = NULL;
    struct sigaction act;
    sigset_t set;

    struct mq_attr rcv_attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = FULL_MSG_SIZE,
        .mq_curmsgs = 0
    };

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sig_handler;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    act.sa_mask = set;
    ret = sigaction(SIGINT, &act, NULL);
    if (ret == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    qid_rcv = mq_open(Q_SERV_RCV, O_RDONLY | O_CREAT | O_EXCL, 0644, &rcv_attr);
    if(qid_rcv == -1){
        perror("mq_open rcv error");
        exit(EXIT_FAILURE);
    }

    while(1){
        ret = mq_receive(qid_rcv, message, rcv_attr.mq_msgsize, &prior);
        if(ret == -1){
            perror("Receive error");
            mq_close(qid_rcv);
            mq_unlink(Q_SERV_RCV);
            free_users(&uhead);
            free_history(&hhead);
            exit(EXIT_FAILURE);
        }
        switch (prior)
        {
            case 1:
                printf("1 %s\n", message);
                add_message(&hhead, &htail, message);
                ret = new_message(&uhead, message);
                if(ret == -1){
                    printf("New message error\n");
                    mq_close(qid_rcv);
                    mq_unlink(Q_SERV_RCV);
                    free_users(&uhead);
                    free_history(&hhead);
                    exit(EXIT_FAILURE);
                }
                break;
            case 2:
                printf("2 %s\n", message);
                ret = add_user(&hhead, &uhead, &utail, message);
                if(ret == -1){
                    printf("Add user error!\n");
                    mq_close(qid_rcv);
                    mq_unlink(Q_SERV_RCV);
                    free_users(&uhead);
                    free_history(&hhead);
                    exit(EXIT_FAILURE);
                }
                ret = user_change(&uhead, message, 2);
                if(ret == -1){
                    printf("User change 1 error\n");
                    mq_close(qid_rcv);
                    mq_unlink(Q_SERV_RCV);
                    free_users(&uhead);
                    free_history(&hhead);
                    exit(EXIT_FAILURE);
                }
                break;
            case 3:
                printf("3 %s\n", message);
                ret = delete_user(&uhead, &utail, message);
                if(ret == -1){
                    printf("User delete error\n");
                    mq_close(qid_rcv);
                    mq_unlink(Q_SERV_RCV);
                    free_users(&uhead);
                    free_history(&hhead);
                    exit(EXIT_FAILURE);
                }
                ret = user_change(&uhead, message, 3);
                if(ret == -1){
                    printf("User change 2 error\n");
                    mq_close(qid_rcv);
                    mq_unlink(Q_SERV_RCV);
                    free_users(&uhead);
                    free_history(&hhead);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                break;
        }
    }
}