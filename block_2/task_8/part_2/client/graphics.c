#include <curses.h>
#include <mqueue.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <string.h>
#include "lists.h"
#include "graphics.h"

void sig_winch(int signo){
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
}

void refresh_msg(WINDOW *win, struct history_list **tail, int count, int limit){
    werase(win);
    struct history_list *tmp = *tail;
    // Отображаем количество записей, определенное лимитом
    for(int i = 0; i < count - 1 && i < limit - 1; i++){
            tmp = tmp->prev;
    }
    while(tmp != NULL){
        wprintw(win, "%s\n", tmp->msg);
        tmp = tmp->next;
    }
    wrefresh(win);
}


void refresh_users(WINDOW *win, struct users_list **tail, int count, int limit){
    werase(win);
    struct users_list *tmp = *tail;
    // Отображаем количество записей, определенное лимитом
    for(int i = 0; i < count - 1 && i < limit - 1; i++){
            tmp = tmp->prev;
    }
    while(tmp != NULL){
        wprintw(win, "%s\n", tmp->username);
        tmp = tmp->next;
    }
    wrefresh(win);
}

void *thread_job(void *arg){
    mqd_t *qid_rcv = (mqd_t*) arg;
    char rcv_msg[FULL_MSG_SIZE];
    unsigned int prior;
    int msg_count = 0;
    int user_count = 0;
    int limit = 38;
    int ret = 0;
    while(1){
        ret = mq_receive(*qid_rcv, rcv_msg, sizeof(rcv_msg), &prior);
        if(ret == -1){
            flag = 0;
            perror("Receive thread error\n");
            pthread_exit(NULL);
        }
        switch(prior)
        {
            case 1:
                ret = add_message(&hhead, &htail, rcv_msg);
                if(ret == -1){
                    flag = 0;
                    printf("Add message thread error\n");
                    pthread_exit(NULL);
                }
                msg_count++;
                refresh_msg(subwnd1, &htail, msg_count, limit);
                break;
            case 2:
                ret = add_user(&uhead, &utail, rcv_msg);
                if(ret == -1){
                    flag = 0;
                    printf("Add user thread error\n");
                    pthread_exit(NULL);
                }
                user_count++;
                refresh_users(subwnd2, &utail, user_count, limit);
                break;
            case 3:
                if(strcmp(rcv_msg, username) == 0){
                    flag = 0;
                    pthread_exit(NULL);
                }
                ret = delete_user(&uhead, &utail, rcv_msg);
                if(ret == -1){
                    flag = 0;
                    printf("Delete user thread error\n");
                    pthread_exit(NULL);
                }
                user_count--;
                refresh_users(subwnd2, &utail, user_count, limit);
                break;
            default:
                break;
        }
    }
}