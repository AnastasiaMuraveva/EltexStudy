#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "lists.h"

int add_user(struct users_list **head, struct users_list **tail, char *msg){
    struct users_list *tmp = calloc(1, sizeof(struct users_list));
    if(tmp == NULL){
        perror("Calloc add user error");
        return -1;
    }

    strncpy(tmp->username, msg, strlen(msg) + 1);

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
    struct users_list *tmp = *head;
    while(strcmp(tmp->username, msg) && tmp){
        tmp = tmp->next;
    }
    if(tmp == NULL){
        printf("Delete user error\n");
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