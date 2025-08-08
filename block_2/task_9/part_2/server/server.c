#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include "lists.h"

sem_t *sem_snd, *sem_rcv;
struct srv_snd *server_send;
struct srv_rcv *server_receive;
int flag = 1;

// Работа сервера зеавершается прерыванием с клавиатуры

void sig_handler(int num, siginfo_t *info, void *args){
    printf("\nReceived signal #%d from %d\n", num, info->si_pid);
    flag = 0;
}

int main(){
    int fd_snd, fd_rcv;
    int ret = 0;
    struct users_list *uhead = NULL;
    struct users_list *utail = NULL;
    struct history_list *hhead = NULL;
    struct history_list *htail = NULL;
    struct sigaction act;
    sigset_t set;

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

    //shm_unlink(SRV_SEND);
    //shm_unlink(SRV_RCV);
    //sem_unlink(SEM_SEND);
    //sem_unlink(SEM_RCV);


    fd_snd = shm_open(SRV_SEND, O_CREAT | O_EXCL | O_RDWR, 0644);
    if(fd_snd == -1){
        perror("shm open 1 error");
        exit(EXIT_FAILURE);
    }
    sem_snd = sem_open(SEM_SEND, O_CREAT | O_EXCL, 0644, 1);
    if(sem_snd == NULL){
        close(fd_snd);
        perror("sem open 1 error");
        exit(EXIT_FAILURE);
    }
    ret = ftruncate(fd_snd, sizeof(struct srv_snd));
    if(ret == -1){
        close(fd_snd);
        sem_close(sem_snd);
        perror("ftruncate 1 error");
        exit(EXIT_FAILURE);
    }
    server_send = mmap(NULL, sizeof(struct srv_snd), PROT_READ | PROT_WRITE, MAP_SHARED, fd_snd, 0);
    if(server_send == NULL){
        close(fd_snd);
        sem_close(sem_snd);
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    fd_rcv = shm_open(SRV_RCV, O_CREAT | O_EXCL | O_RDWR, 0644);
    if(fd_rcv == -1){
        munmap(server_send, sizeof(struct srv_snd));
        close(fd_snd);
        sem_close(sem_snd);
        perror("shm open 2 error");
        exit(EXIT_FAILURE);
    }
    sem_rcv = sem_open(SEM_RCV, O_CREAT | O_EXCL, 0644, 1);
    if(sem_rcv == NULL){
        close(fd_rcv);
        munmap(server_send, sizeof(struct srv_snd));
        close(fd_snd);
        sem_close(sem_snd);
        perror("sem open 2 error");
        exit(EXIT_FAILURE);
    }
    ret = ftruncate(fd_rcv, RCV_SIZE * sizeof(struct srv_rcv));
    if(sem_rcv == NULL){
        close(fd_rcv);
        munmap(server_send, sizeof(struct srv_snd));
        close(fd_snd);
        sem_close(sem_snd);
        sem_close(sem_rcv);
        perror("ftruncate 2 error");
        exit(EXIT_FAILURE);
    }
    server_receive = mmap(NULL, RCV_SIZE * sizeof(struct srv_rcv), PROT_READ | PROT_WRITE, MAP_SHARED, fd_rcv, 0);
    if(sem_rcv == NULL){
        close(fd_rcv);
        munmap(server_send, sizeof(struct srv_snd));
        close(fd_snd);
        sem_close(sem_snd);
        sem_close(sem_rcv);
        perror("mmap 2 error");
        exit(EXIT_FAILURE);
    }

    while(1){
        // Если в памяти ничего нет, ждем
        while(server_receive[0].message[0] ==  0 && flag){
            continue;
        }
        if(flag == 0){
            printf("End of work\n");
            break;
        }
        ret = sem_wait(sem_rcv);
        for(int i = 0; i < RCV_SIZE; i++){
            if(server_receive[i].message[0] == 0)
                break;
            printf("%d %s\n", server_receive[i].type, server_receive[i].message);
            switch (server_receive[i].type)
            {
                case 1:
                    ret = add_message(&hhead, &htail, server_receive[i].message);
                    if(ret == -1){
                        printf("add message error\n");
                        free_history(&hhead);
                        free_users(&uhead);
                        close_all();
                        exit(EXIT_FAILURE);
                    }
                    ret = new_message(&uhead, server_receive[i].message, server_receive[i].type);
                    if(ret == -1){
                        printf("new message 1 error\n");
                        free_history(&hhead);
                        free_users(&uhead);
                        close_all();
                        exit(EXIT_FAILURE);
                    }
                    memset(server_receive[i].message, 0, sizeof(struct srv_rcv));
                    break;
                case 2:
                    ret = add_user(&hhead, &uhead, &utail, server_receive[i].message);
                    if(ret == -1){
                        printf("add user error\n");
                        free_history(&hhead);
                        free_users(&uhead);
                        close_all();
                        exit(EXIT_FAILURE);
                    }
                    if(ret != 1){
                        ret = new_message(&uhead, server_receive[i].message, server_receive[i].type);
                        if(ret == -1){
                            printf("new message 2 error\n");
                            free_history(&hhead);
                            free_users(&uhead);
                            close_all();
                            exit(EXIT_FAILURE);
                        }
                    }
                    memset(server_receive[i].message, 0, sizeof(struct srv_rcv));
                    break;
                case 3:
                    ret = delete_user(&uhead, &utail, server_receive[i].message);
                    if(ret == -1){
                        printf("delete user error\n");
                        free_history(&hhead);
                        free_users(&uhead);
                        close_all();
                        exit(EXIT_FAILURE);
                    }
                    ret = new_message(&uhead, server_receive[i].message, server_receive[i].type);
                    if(ret == -1){
                        printf("new message 3 error\n");
                        free_history(&hhead);
                        free_users(&uhead);
                        close_all();
                        exit(EXIT_FAILURE);
                    }
                    memset(server_receive[i].message, 0, sizeof(struct srv_rcv));
                    break;
                default:
                    break;

            }
        }
        ret = sem_post(sem_rcv);
    }
    free_history(&hhead);
    free_users(&uhead);
    close_all();
    exit(EXIT_SUCCESS);
}
