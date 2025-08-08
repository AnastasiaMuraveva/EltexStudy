#ifndef __LISTS_H__
#define __LISTS_H__

#include <semaphore.h>

#define NAME_SIZE 30
#define MSG_SIZE 100
#define FULL_MSG_SIZE 135
#define RCV_SIZE 200

#define SRV_SEND "/serv_shmsend"
#define SRV_RCV "/serv_shmrcv"

#define SEM_SEND "/sem_send"
#define SEM_RCV "/sem_rcv"

#define ERROR_FUNC(str, status, ret, exit_status) if(ret == status) { \
                                        munmap(server_send, sizeof(struct srv_snd)); \
                                        munmap(server_receive, RCV_SIZE * sizeof(struct srv_rcv)); \
                                        sem_close(sem_snd); \
                                        sem_close(sem_rcv); \
                                        free_users(&uhead); \
                                        free_history(&hhead); \
                                        delwin(subwnd1); \
                                        delwin(subwnd2); \
                                        delwin(subwnd3); \
                                        delwin(wnd1); \
                                        delwin(wnd2); \
                                        delwin(wnd3); \
                                        endwin(); \
                                        perror(str); \
                                        exit(exit_status); \
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
extern char username[NAME_SIZE];

// Функция добавления сообщения в список
int add_message(struct history_list **head, struct history_list **tail);

// Функция освобождения списка сообщений
void free_history(struct history_list **head);

// Функция добавления пользователя в список
int add_user(struct users_list **head, struct users_list **tail);

// Функция удаления пользователя из списка
int delete_user(struct users_list **head, struct users_list **tail);

// Функция освобождения списка пользователей
void free_users(struct users_list **head);

#endif