#include <termios.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <curses.h>
#include <unistd.h>
#include <signal.h>
#include "lists.h"
#include "graphics.h"
#define Q_CLIENT_SND "/serv_rcv"

// Работа клиента завершается вводом слова "exit" в поле для ввода сообщений
// Курсор ввода возвращается в поле для ввода при нажатии любой клавиши

struct users_list *uhead = NULL;
struct users_list *utail = NULL;
struct history_list *hhead = NULL;
struct history_list *htail = NULL;
WINDOW * wnd1, *wnd2, *wnd3;
WINDOW * subwnd1, *subwnd2, *subwnd3;
char username[NAME_SIZE];
int flag = 1;

int main(){
    mqd_t qid_rcv, qid_snd;
    char qname[QNAME_SIZE];
    char input[MSG_SIZE];
    char snd_msg[FULL_MSG_SIZE];
    pthread_t thread;
    int ret = 0;

    struct wnd_info wnd1_info = {40, 140, 2, 4};
    struct wnd_info wnd2_info = {40, 31, 2, 150};
    struct wnd_info wnd3_info = {5, 177, 43, 4};

    struct wnd_info subwnd1_info = {38, 138, 1, 1};
    struct wnd_info subwnd2_info = {38, 29, 1, 1};
    struct wnd_info subwnd3_info = {3, 175, 1, 1};

    initscr();
    signal(SIGWINCH, sig_winch);
    keypad(stdscr, TRUE);
    //curs_set(TRUE);
    refresh();

    wnd1 = newwin(wnd1_info.y, wnd1_info.x, wnd1_info.y_offset, wnd1_info.x_offset);
    box(wnd1, '|', '-');
    wrefresh(wnd1);
    subwnd1 = derwin(wnd1, subwnd1_info.y, subwnd1_info.x, subwnd1_info.y_offset, subwnd1_info.x_offset);

    wnd2 = newwin(wnd2_info.y, wnd2_info.x, wnd2_info.y_offset, wnd2_info.x_offset);
    box(wnd2, '|', '-');
    wrefresh(wnd2);
    subwnd2 = derwin(wnd2, subwnd2_info.y, subwnd2_info.x, subwnd2_info.y_offset, subwnd2_info.x_offset);

    wnd3 = newwin(wnd3_info.y, wnd3_info.x, wnd3_info.y_offset, wnd3_info.x_offset);
    box(wnd3, '|', '-');
    wrefresh(wnd3);
    subwnd3 = derwin(wnd3, subwnd3_info.y, subwnd3_info.x, subwnd3_info.y_offset, subwnd3_info.x_offset);

    wprintw(subwnd3, "Enter your name:\n");
    wrefresh(subwnd3);
    while(1){
        wgetnstr(subwnd3, username, NAME_SIZE - 1);
        username[NAME_SIZE - 1] = 0;
        if(strlen(username) == 0){
            werase(subwnd3);
            wprintw(subwnd3, "Empty string! Enter again:\n");
            wrefresh(subwnd3);
            continue;
        }
        snprintf(qname, sizeof(qname), "/%s", username);
        ret = mq_open(qname, O_RDONLY, 0644, NULL);
        if(ret != -1){
            mq_close(ret);
            werase(subwnd3);
            wprintw(subwnd3, "Name is taken! Enter again:\n");
            wrefresh(subwnd3);
            continue;
        }
        break;
    }

    qid_snd = mq_open(Q_CLIENT_SND, O_WRONLY, 0644, NULL);
    if(qid_snd == -1){
        delwin(subwnd1);
        delwin(subwnd2);
        delwin(subwnd3);
        delwin(wnd1);
        delwin(wnd2);
        delwin(wnd3);
        endwin();
        perror("Open error");
        exit(EXIT_FAILURE);
    }

    ret = mq_send(qid_snd, username, strlen(username) + 1, 2);
    if(ret == -1){
        mq_close(qid_snd);
        delwin(subwnd1);
        delwin(subwnd2);
        delwin(subwnd3);
        delwin(wnd1);
        delwin(wnd2);
        delwin(wnd3);
        endwin();
        perror("Send 1 error");
        exit(EXIT_FAILURE);
    }
    while(1){
        qid_rcv = mq_open(qname, O_RDONLY, 0644, NULL);
        if(qid_rcv == -1){
            if(errno == ENOENT){
                continue;
            }
            mq_close(qid_snd);
            delwin(subwnd1);
            delwin(subwnd2);
            delwin(subwnd3);
            delwin(wnd1);
            delwin(wnd2);
            delwin(wnd3);
            endwin();
            perror("Open error");
            exit(EXIT_FAILURE);
        }
        break;
    }

    pthread_create(&thread, NULL, thread_job, (void*) &qid_rcv);

    while(flag){
        werase(subwnd3);
        wrefresh(subwnd3);
        wgetnstr(subwnd3, input, MSG_SIZE - 1);
        input[MSG_SIZE - 1] = 0;

        // Условие выхода из приложения
        if(strcmp(input, "exit") == 0){
            break;
        }

        snprintf(snd_msg, sizeof(snd_msg), "%s: %s", username, input);

        ret = mq_send(qid_snd, snd_msg, strlen(snd_msg) + 1, 1);
        if(ret == -1){
            werase(subwnd3);
            wprintw(subwnd3, "Cannot send message to server\n");
            wrefresh(subwnd3);
            wgetch(subwnd3);
            break;
        }
    }

    ret = mq_send(qid_snd, username, strlen(username) + 1, 3);
    if(ret == -1)
        perror("Send error");
   
    pthread_join(thread, NULL);
    mq_close(qid_snd);
    mq_close(qid_rcv);
    free_users(&uhead);
    free_history(&hhead);
    delwin(subwnd1);
    delwin(subwnd2);
    delwin(subwnd3);
    delwin(wnd1);
    delwin(wnd2);
    delwin(wnd3);
    endwin();
    exit(EXIT_SUCCESS);
}
