#include <termios.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <curses.h>
#include <unistd.h>
#include <signal.h>
#include <err.h>
#include <fcntl.h>
#include "lists.h"
#include "graphics.h"
#define Q_CLIENT_SND "/serv_rcv"

// Работа клиента завершается вводом слова "exit" в поле для ввода сообщений
// Курсор ввода возвращается в поле для ввода при нажатии любой клавиши

// Периодически сбивается графика, после очередной отправки сообщения все приходит в норму, 
// не знаю, в чем причина

struct users_list *uhead = NULL;
struct users_list *utail = NULL;
struct history_list *hhead = NULL;
struct history_list *htail = NULL;
WINDOW * wnd1, *wnd2, *wnd3;
WINDOW * subwnd1, *subwnd2, *subwnd3;

sem_t *sem_snd, *sem_rcv;
struct srv_snd *server_send;
struct srv_rcv *server_receive;

char username[NAME_SIZE] = {0};
int flag = 1;

int main(){
    int fd_snd, fd_rcv;
    char input[MSG_SIZE] = { 0 };
    char snd_msg[FULL_MSG_SIZE] = { 0 };
    pthread_t thread;
    int ret = 0;

    struct wnd_info wnd1_info = {40, 140, 2, 4};
    struct wnd_info wnd2_info = {40, 31, 2, 150};
    struct wnd_info wnd3_info = {5, 177, 43, 4};

    struct wnd_info subwnd1_info = {38, 138, 1, 1};
    struct wnd_info subwnd2_info = {38, 29, 1, 1};
    struct wnd_info subwnd3_info = {3, 175, 1, 1};

    while(1){
        fd_snd = shm_open(SRV_SEND, O_RDWR, 0644);
        if(fd_snd == -1){
            if(errno = ENOENT)
                continue;
            err(EXIT_FAILURE, "shm open 1 error");
        }
        break;
    }
    while(1){
        sem_snd = sem_open(SEM_SEND, 0);
        if(sem_snd == NULL){
            if(errno == ENOENT){
                continue;
            }
            close(fd_snd);
            err(EXIT_FAILURE, "sem open 1 error");
        }
        break;
    }
    
    ret = ftruncate(fd_snd, sizeof(struct srv_snd));
    if(ret == -1){
        close(fd_snd);
        sem_close(sem_snd);
        err(EXIT_FAILURE, "ftruncate 1 error");
    }
    server_send = mmap(NULL, sizeof(struct srv_snd), PROT_READ | PROT_WRITE, MAP_SHARED, fd_snd, 0);
    if(server_send == NULL){
        close(fd_snd);
        sem_close(sem_snd);
        err(EXIT_FAILURE, "mmap 1 error");
    }

    while(1){
        fd_rcv = shm_open(SRV_RCV, O_RDWR, 0644);
        if(fd_rcv == -1){
            if(errno == ENOENT){
                continue;
            }
            munmap(server_send, sizeof(struct srv_snd));
            close(fd_snd);
            sem_close(sem_snd);
            err(EXIT_FAILURE, "shm open 2 error");
        }
        break;
    }
    
    while(1){
        sem_rcv = sem_open(SEM_RCV, 0);
        if(sem_rcv == NULL){
            if(errno == ENOENT)
                continue;
            close(fd_rcv);
            munmap(server_send, sizeof(struct srv_snd));
            close(fd_snd);
            sem_close(sem_snd);
            err(EXIT_FAILURE, "sem open 2 error");
        }
        break;
    }
    ret = ftruncate(fd_rcv, RCV_SIZE * sizeof(struct srv_rcv));
    if(ret == -1){
        sem_close(sem_rcv);
        close(fd_rcv);
        munmap(server_send, sizeof(struct srv_snd));
        close(fd_snd);
        sem_close(sem_snd);
        err(EXIT_FAILURE, "ftruncate 2 error");
    }
    server_receive = mmap(NULL, RCV_SIZE * sizeof(struct srv_rcv), PROT_READ | PROT_WRITE, MAP_SHARED, fd_rcv, 0);
    if(server_receive == NULL){
        sem_close(sem_rcv);
        close(fd_rcv);
        munmap(server_send, sizeof(struct srv_snd));
        close(fd_snd);
        sem_close(sem_snd);
        err(EXIT_FAILURE, "ftruncate 2 error");
    }

    initscr();
    signal(SIGWINCH, sig_winch);
    keypad(stdscr, TRUE);
    curs_set(TRUE);
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
        break;
    }

    // Отправляем имя серверу
    for(int i = 0; i < RCV_SIZE; i++){
        ret = sem_wait(sem_rcv);
        ERROR_FUNC("sem wait 1 error", -1, ret, EXIT_FAILURE);
        if(strlen(server_receive[i].message) == 0){
            strncpy(server_receive[i].message, username, strlen(username) + 1);
            server_receive[i].type = 2;
            ret = sem_post(sem_rcv);
            ERROR_FUNC("sem post 1 error", -1, ret, EXIT_FAILURE);
            break;
        }
        ret = sem_post(sem_rcv);
        ERROR_FUNC("sem post 1 error", -1, ret, EXIT_FAILURE);
    }
    // Ожидаем ответа от сервера о принятии или отклонении входа
    while(server_send->type != 4 || strcmp(server_send->name, username) != 0){
        continue;
    }
    // Если имя занято, завершаем работу приложения
    ERROR_FUNC("Name is taken", 0, strcmp(server_send->message, "no"), EXIT_FAILURE);

    // Если имя не занято отправляем серверу ответ о том что сообщение прочитано
    ret = sem_wait(sem_snd);
    ERROR_FUNC("sem wait 2 error", -1, ret, EXIT_FAILURE);
    server_send->cond = 1;
    ret = sem_post(sem_snd);
    ERROR_FUNC("sem post 2 error", -1, ret, EXIT_FAILURE);

    pthread_create(&thread, NULL, thread_job, NULL);

    while(flag){
        werase(subwnd3);
        wrefresh(subwnd3);
        wgetnstr(subwnd3, input, MSG_SIZE - 1);
        input[MSG_SIZE - 1] = 0;

        // Условие выхода из приложения
        if(strcmp(input, "exit") == 0){
            break;
        }

        snprintf(snd_msg, FULL_MSG_SIZE, "%s: %s", username, input);

        for(int i = 0; i < RCV_SIZE; i++){
            ret = sem_wait(sem_rcv);
            ERROR_FUNC("sem wait 3 error", -1, ret, EXIT_FAILURE);
            if(strlen(server_receive[i].message) == 0){
                strncpy(server_receive[i].message, snd_msg, strlen(snd_msg) + 1);
                server_receive[i].type = 1;
                ret = sem_post(sem_rcv);
                ERROR_FUNC("sem post 3 error", -1, ret, EXIT_FAILURE);
                break;
            }
            ret = sem_post(sem_rcv);
            ERROR_FUNC("sem post 3 error", -1, ret, EXIT_FAILURE);
        }
    }

    for(int i = 0; i < RCV_SIZE; i++){
        ret = sem_wait(sem_rcv);
        ERROR_FUNC("sem wait 4 error", -1, ret, EXIT_FAILURE);
        if(server_receive[i].message[0] == 0){
            strncpy(server_receive[i].message, username, strlen(username) + 1);
            server_receive[i].type = 3;
            ret = sem_post(sem_rcv);
            ERROR_FUNC("sem post 4 error", -1, ret, EXIT_FAILURE);
            break;
        }
        ret = sem_post(sem_rcv);
        ERROR_FUNC("sem post 4 error", -1, ret, EXIT_FAILURE);
    }
    ret = pthread_join(thread, NULL);
    ERROR_FUNC("join error", -1, ret, EXIT_FAILURE);
    ERROR_FUNC("The process terminated correctly", 0, ret, EXIT_SUCCESS);
}