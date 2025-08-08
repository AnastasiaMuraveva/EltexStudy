#ifndef __LISTS_H__
#define __LISTS_H__
#include <mqueue.h>
#include <semaphore.h>

#define NAME_SIZE 30
#define FULL_MSG_SIZE 135
#define RCV_SIZE 200

#define SRV_SEND "/serv_shmsend"
#define SRV_RCV "/serv_shmrcv"

#define SEM_SEND "/sem_send"
#define SEM_RCV "/sem_rcv"

#define ERROR_FUNC(str, status, ret) if(ret == status) { \
                                        perror(str); \
                                        close_all(); \
                                        exit(EXIT_FAILURE); \
                                    }

struct srv_snd{
    char message[FULL_MSG_SIZE];
    int type;
    int cond;
    char name[NAME_SIZE];
};

struct srv_rcv{
    char message[FULL_MSG_SIZE];
    int type;
};


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

extern sem_t *sem_snd, *sem_rcv;
extern struct srv_snd *server_send;
extern struct srv_rcv *server_receive;

// Функция добавления пользователя в список
int add_user(struct history_list **hhead, struct users_list **head, struct users_list **tail, char *msg);

// Функция удаления пользователя из списка
int delete_user(struct users_list **head, struct users_list **tail, char *msg);

// Функция освобождения списка пользователей
void free_users(struct users_list **head);

// Функция извещения клиентов о новом сообщении
int new_message(struct users_list **head, char *msg, int type);

// Функция добавления сообщения в список
int add_message(struct history_list **head, struct history_list **tail, char *msg);

// Функция освобождения списка сообщений
void free_history(struct history_list **head);

void close_all(void);

#endif