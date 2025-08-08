#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "lists.h"

int add_user(struct users_list **head, struct users_list **tail){
    int ret = 0;
    struct users_list *tmp = calloc(1, sizeof(struct users_list));
    if(tmp == NULL){
        perror("Calloc add user error");
        return -1;
    }

    ret = sem_wait(sem_snd);
    if(ret == -1){
        perror("sem wait add user error");
        free(tmp);
        return -1;
    }

    // Убираем свое имя, чтобы не прочитать сообщение второй раз
    // Помечаем сообщение прочитанным
    strncpy(tmp->username, server_send->message, NAME_SIZE);
    memset(server_send->name, 0, NAME_SIZE);
    server_send->cond++;
    ret = sem_post(sem_snd);
    if(ret == -1){
        perror("sem post add user error");
        free(tmp);
        return -1;
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

int delete_user(struct users_list **head, struct users_list **tail){
    int ret = 0;
    struct users_list *tmp = *head;
    while(strcmp(tmp->username, server_send->message) && tmp){
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

    // Убираем свое имя, чтобы не прочитать сообщение второй раз
    // Помечаем сообщение прочитанным
    memset(server_send->name, 0, NAME_SIZE);
    server_send->cond++;
    ret = sem_post(sem_snd);
    if(ret == -1){
        perror("sem post delete user error");
        return -1;
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

void free_users(struct users_list **head){
    struct users_list *current;
    if(*head == NULL)
        return;
    while(*head != NULL){
        current = *head;
        *head = (*head)->next;
        free(current);
    }
}

int add_message(struct history_list **head, struct history_list **tail){
    int ret = 0;
    struct history_list *tmp = calloc(1, sizeof(struct history_list));
    if(tmp == NULL){
        perror("Calloc add message error");
        return -1;
    }

    ret = sem_wait(sem_snd);
    if(ret == -1){
        perror("sem wait add message error");
        free(tmp);
        return -1;
    }

    // Убираем свое имя, чтобы не прочитать сообщение второй раз
    // Помечаем сообщение прочитанным
    strncpy(tmp->msg, server_send->message, strlen(server_send->message) + 1);
    memset(server_send->name, 0, NAME_SIZE);
    server_send->cond++;
    ret = sem_post(sem_snd);
    if(ret == -1){
        perror("sem post add message error");
        free(tmp);
        return -1;
    }

    // Если список пуст
    if(*head == NULL){
        *head = tmp;
        (*head)->prev = NULL;
        *tail = tmp;
    }
    // Если в списке один элемент
    else if((*head)->next == NULL){
        tmp->prev = *head;
        tmp->next = NULL;
        (*head)->next = tmp;
        *tail = tmp;
    }
    // Если в списке > 1 элемента
    else{
        tmp->next = NULL;
        tmp->prev = *tail;
        (*tail)->next = tmp;
        *tail = tmp;
    }
    return 0;
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