#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <curses.h>
#include "lists.h"

// Информация о размере и расположении окна 
struct wnd_info{
    int y;
    int x;
    int y_offset;
    int x_offset;
};

// Указатели на списки
extern struct users_list *uhead;
extern struct users_list *utail;
extern struct history_list *hhead;
extern struct history_list *htail;
// Окна для отображения списков
extern WINDOW * wnd1, *wnd2;
extern WINDOW * subwnd1, *subwnd2;
// Имя пользователя
extern char username[NAME_SIZE];
// Флаг для выхода из основного цикла в случае завершения дочернего потока
extern int flag;

void sig_winch(int signo);

// Обновление экрана сообщений
void refresh_msg(WINDOW *win, struct history_list **tail, int count, int limit);

// Обновление экрана пользователей
void refresh_users(WINDOW *win, struct users_list **tail, int count, int limit);

// Функция получения сообщений от сервера
void *thread_job(void *arg);

#endif