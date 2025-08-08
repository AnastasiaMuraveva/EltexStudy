#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "lists.h"

int add_user(struct history_list **hhead, struct users_list **head, struct users_list **tail, char *msg){
    struct users_list *tmp = calloc(1, sizeof(struct users_list));
    struct users_list *cur;
    struct history_list *htmp = *hhead;
    int ret = 0;
    if(tmp == NULL){
        perror("Add user calloc error\n");
        return -1;
    }

    snprintf(tmp->data.qname, sizeof(tmp->data.qname), "/%s", msg);

    tmp->data.qattr.mq_flags = 0;
    tmp->data.qattr.mq_maxmsg = 10;
    tmp->data.qattr.mq_msgsize = FULL_MSG_SIZE;
    tmp->data.qattr.mq_curmsgs = 0;

    strncpy(tmp->data.username, msg, NAME_SIZE);

    tmp->data.qid = mq_open(tmp->data.qname, O_WRONLY | O_CREAT | O_EXCL, 0644, &tmp->data.qattr);
    if(tmp->data.qid == -1){
        perror("Add user open error");
        return -1;
    }

    // Отправляем историю сообщений
    while(htmp != NULL){
        ret = mq_send(tmp->data.qid, htmp->msg, strlen(htmp->msg) + 1, 1);
        if(ret == -1){
            perror("Add user send 1 error");
            return -1;
        }
        htmp = htmp->next;
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

    // Отпраляем информацию о пользователях
    cur = *head;
    while(cur != NULL){
        ret = mq_send(tmp->data.qid, cur->data.username, strlen(cur->data.username) + 1, 2);
        if(ret == -1){
            perror("Add user send 2 error");
            return -1;
        }
        cur = cur->next;
    }
    return 0;
}


int delete_user(struct users_list **head, struct users_list **tail, char *msg){
    int ret = 0;
    struct users_list *tmp = *head;

    while(strcmp(tmp->data.username, msg) && tmp){
        tmp = tmp->next;
    }
    if(tmp == NULL){
        printf("Delete user error\n");
        return -1;
    }

    // Отправляем пользователю сообщение об его удалении для выхода из дочернего потока
    ret = mq_send(tmp->data.qid, msg, strlen(msg) + 1, 3);
    if(ret == -1){
        perror("Delete user send error");
        return -1;
    }

    ret = mq_close(tmp->data.qid);
    if(ret == -1){
        perror("Delete user close error");
        return -1;
    }
    ret = mq_unlink(tmp->data.qname);
    if(ret == -1){
        perror("Delete user ulink error");
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

int user_change(struct users_list **head, char *msg, int type){
    struct users_list *tmp = *head;
    int ret = 0;
    while(tmp != NULL){
        if(strcmp(msg, tmp->data.username) != 0){
            ret = mq_send(tmp->data.qid, msg, strlen(msg) + 1, type);
        }
        if(ret == -1){
            perror("Send users error");
            return -1;
        }
        tmp = tmp->next;
    }
    return 0;
}


int new_message(struct users_list **head, char *msg){
    struct users_list *tmp = *head;
    int ret = 0;
    while(tmp != NULL){
        ret = mq_send(tmp->data.qid, msg, strlen(msg) + 1, 1);
        if(ret == -1){
            perror("Send new message error");
            return -1;
        }
        tmp = tmp->next;
    }
    return 0;
}

void free_users(struct users_list **head){
    struct users_list *current;
    if(*head == NULL)
        return;
    while(*head != NULL){
        current = *head;
        *head = (*head)->next;
        mq_send(current->data.qid, current->data.username, strlen(current->data.username) + 1, 3);
        mq_close(current->data.qid);
        mq_unlink(current->data.qname);
        free(current);
    }
}