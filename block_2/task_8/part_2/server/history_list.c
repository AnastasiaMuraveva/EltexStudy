#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lists.h"

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