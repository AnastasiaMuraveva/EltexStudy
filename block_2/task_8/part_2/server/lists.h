#ifndef __USER_LIST_H__
#define __USER_LIST_H__
#include <mqueue.h>

#define NAME_SIZE 30
#define QNAME_SIZE 31
#define MSG_SIZE 100
#define FULL_MSG_SIZE 135


struct history_list{
    struct history_list *next;
    struct history_list *prev;
    char msg[FULL_MSG_SIZE];
};

struct user_info{
    char username[NAME_SIZE];
    char qname[QNAME_SIZE];
    mqd_t qid;
    struct mq_attr qattr;
};

struct users_list{
    struct users_list *next;
    struct users_list *prev;
    struct user_info data;
};

// Функция добавления пользователя в список
int add_user(struct history_list **hhead, struct users_list **head, struct users_list **tail, char *msg);

// Функция удаления пользователя из списка
int delete_user(struct users_list **head, struct users_list **tail, char *msg);

// Функция извещения клиентов об изменении количества пользователей
int user_change(struct users_list **head, char *msg, int type);

// Функция освобождения списка пользователей
void free_users(struct users_list **head);

// Функция извещения клиентов о новом сообщении
int new_message(struct users_list **head, char *msg);

// Функция добавления сообщения в список
int add_message(struct history_list **head, struct history_list **tail, char *msg);

// Функция освобождения списка сообщений
void free_history(struct history_list **head);

#endif