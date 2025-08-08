#ifndef __HISTORY_LIST_H__
#define __HISTORY_LIST_H__
#define NAME_SIZE 30
#define QNAME_SIZE 31
#define MSG_SIZE 100
#define FULL_MSG_SIZE 135

struct history_list{
    struct history_list *next;
    struct history_list *prev;
    char msg[FULL_MSG_SIZE];
};

struct users_list{
    struct users_list *next;
    struct users_list *prev;
    char username[NAME_SIZE];
};

// Функция добавления сообщения в список
int add_message(struct history_list **head, struct history_list **tail, char *msg);

// Функция освобождения списка сообщений
void free_history(struct history_list **head);

// Функция добавления пользователя в список
int add_user(struct users_list **head, struct users_list **tail, char *msg);

// Функция удаления пользователя из списка
int delete_user(struct users_list **head, struct users_list **tail, char *msg);

// Функция освобождения списка пользователей
void free_users(struct users_list **head);

#endif