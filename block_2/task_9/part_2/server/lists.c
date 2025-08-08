#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "lists.h"

void close_all(void){
    munmap(server_send, sizeof(struct srv_snd));
    shm_unlink(SRV_SEND);

    munmap(server_receive, RCV_SIZE * sizeof(struct srv_rcv));
    shm_unlink(SRV_RCV);

    sem_close(sem_snd);
    sem_unlink(SEM_SEND);

    sem_close(sem_rcv);
    sem_unlink(SEM_RCV);
}

int add_user(struct history_list **hhead, struct users_list **head, struct users_list **tail, char *msg){
    struct users_list *utmp = *head;
    struct users_list *tmp;
    struct history_list *htmp = *hhead;
    int ret = 0;

    // Проверяем, занято ли имя
    while(utmp != NULL){
            // Если занято отправляем отказ
            if(strcmp(utmp->username, msg) == 0){
                ret = sem_wait(sem_snd);
                strncpy(server_send->message, "no", 3);
                strncpy(server_send->name, msg, strlen(msg) + 1);
                server_send->cond = 0;
                server_send->type = 4;
                ret = sem_post(sem_snd); 
                return 1; 
            }
            utmp = utmp->next;
    }

    ret = sem_wait(sem_snd);
    if(ret == -1){
        perror("sem wait 1 add user error");
        return -1;
    }
    // Если не занято - принимаем запрос
    strncpy(server_send->message, "yes", 4);
    strncpy(server_send->name, msg, strlen(msg) + 1);
    server_send->cond = 0;
    server_send->type = 4;
    ret = sem_post(sem_snd); 
    if(ret == -1){
        perror("sem post 1 add user error");
        return -1;
    }
    // Дожидаемся ответа
    while(server_send->cond < 1){
        continue;
    }
    
    tmp = calloc(1, sizeof(struct users_list));
    if(tmp == NULL){
        perror("Add user calloc error\n");
        return -1;
    }

    strncpy(tmp->username, msg, NAME_SIZE);

    // Отправляем историю сообщений
    while(htmp != NULL){
            ret = sem_wait(sem_snd);
            if(ret == -1){
                perror("sem wait 2 add user error");
                free(tmp);
                return -1;
            }
            strncpy(server_send->message, htmp->msg, strlen(htmp->msg) + 1);
            strncpy(server_send->name, msg, strlen(msg) + 1);
            server_send->cond = 0;
            server_send->type = 1;
            ret = sem_post(sem_snd);
            if(ret == -1){
                perror("sem post 2 add user error");
                free(tmp);
                return -1;
            }
            // Дожидаемся ответа
            while(server_send->cond < 1){
                continue;
            }
            htmp = htmp->next;
    }
    // Отправляем текущих пользователей
    utmp = *head;
    while(utmp != NULL){
            ret = sem_wait(sem_snd);
            if(ret == -1){
                perror("sem wait 3 add user error");
                free(tmp);
                return -1;
            }
            strncpy(server_send->message, utmp->username, strlen(utmp->username) + 1);
            strncpy(server_send->name, msg, strlen(msg) + 1);
            server_send->cond = 0;
            server_send->type = 2;
            ret = sem_post(sem_snd);
            if(ret == -1){
                perror("sem post 3 add user error");
                free(tmp);
                return -1;
            }
            // Дожидаемся ответа
            while(server_send->cond < 1){
                continue;
            }
            utmp = utmp->next;
    }

    if(*head == NULL){
        *head = tmp;
        *tail = tmp;
    }
    else if((*head)->next == NULL){
        tmp->prev = *head;
        (*head)->next = tmp;
        *tail = tmp;
    }
    else{
        tmp->prev = *tail;
        (*tail)->next = tmp;
        *tail = tmp;
    }
    return 0;
}

int delete_user(struct users_list **head, struct users_list **tail, char *msg){
    int ret = 0;
    struct users_list *tmp = *head;

    while(strcmp(tmp->username, msg) && tmp){
        tmp = tmp->next;
    }
    if(tmp == NULL){
        printf("Delete user error\n");
        return -1;
    }

    ret = sem_wait(sem_snd);
    if(ret == -1){
        perror("sem wait delete user error");
        return -1;
    }
    strncpy(server_send->message, tmp->username, strlen(tmp->username) + 1);
    strncpy(server_send->name, tmp->username, strlen(tmp->username) + 1);
    server_send->cond = 0;
    server_send->type = 3;
    ret = sem_post(sem_snd);
    if(ret == -1){
        perror("sem post delete user error");
        return -1;
    }
    // Дожидаемся ответа
    while(server_send->cond < 1){
        continue;
    }

    // Если удаляем единственный элемент
    if(*head == *tail){
        *head = NULL;
    }
    // Если удаляем первый элемент
    else if(tmp == *head){
        *head = (*head)->next;
        (*head)->prev = NULL;
    }
    // Если удаляем последний элемент
    else if(tmp == *tail){
        *tail = (*tail)->prev;
        (*tail)->next = NULL;
    }
    // Если удаляем элемент в середине
    else{
        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
    }
    free(tmp);
    return 0;
}

int new_message(struct users_list **head, char *msg, int type){
    struct users_list *tmp = *head;
    int ret = 0;
    while(tmp != NULL){
        ret = sem_wait(sem_snd);
        if(ret == -1){
            perror("sem wait new message error");
            return -1;
        }
        strncpy(server_send->message, msg, strlen(msg) + 1);
        strncpy(server_send->name, tmp->username, strlen(tmp->username) + 1);
        server_send->cond = 0;
        server_send->type = type;
        ret = sem_post(sem_snd);
        if(ret == -1){
            perror("sem post new message error");
            return -1;
        }
        // Дожидаемся ответа
        while(server_send->cond < 1){
            continue;
        }
        tmp = tmp ->next;
    }
    return 0;
}

int add_message(struct history_list **head, struct history_list **tail, char *msg){
    struct history_list *tmp = calloc(1, sizeof(struct history_list));
    if(tmp == NULL){
        perror("Calloc error\n");
        return -1;
    }

    strncpy(tmp->msg, msg, strlen(msg) + 1);

    // Если в справочнике нет записей
    if(*head == NULL){
        *head = tmp;
        *tail = tmp;
    }
    // Если в справочнике одна запись
    else if((*head)->next == NULL){
        tmp->prev = *head;
        (*head)->next = tmp;
        *tail = tmp;
    }
    // Если в справочнике > 1 записи
    else{
        tmp->prev = *tail;
        (*tail)->next = tmp;
        *tail = tmp;
    }
    return 0;
}

void free_users(struct users_list **head){
    struct users_list *current;
    int ret = 0;
    if(*head == NULL)
        return;
    while(*head != NULL){
        current = *head;
        *head = (*head)->next;
        ret = sem_wait(sem_snd);
        if(ret == -1){
            perror("sem wait free users error");
        }
        strncpy(server_send->message, current->username, strlen(current->username) + 1);
        strncpy(server_send->name, current->username, strlen(current->username) + 1);
        server_send->cond = 0;
        server_send->type = 3;
        ret = sem_post(sem_snd);
        if(ret == -1){
            perror("sem post free users error");
        }
        usleep(100);
        free(current);
    }
}

void free_history(struct history_list **head){
    struct history_list *current;
    if(*head == NULL)
        return;
    while(*head != NULL){
        current = *head;
        *head = (*head)->next;
        free(current);
    }
}