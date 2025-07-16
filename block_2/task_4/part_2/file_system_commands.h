#ifndef __FILE_SYSTEM_COMMANDS_H__
#define __FILE_SYSTEM_COMMANDS_H__

#include <curses.h>
#include <dirent.h>

#define STR_LEN 1024

// Информация о размере и расположении окна 
struct wnd_info{
    int y;
    int x;
    int y_offset;
    int x_offset;
};

// Настройки окна
struct wnd_set{
    // Текущая строка
    int cur;
    // Номер первой отображаемой строки(для прокрутки вниз)
    int head;
    // Путь текущей папки
    char path[STR_LEN];
    // Размер структуры dirent
    int count;
    struct dirent **list;
};

void sig_winch(int signo);

// Функция перезаписи текущего пути на предыдущий в иерархии
void prev_dir(char *path);

// Функция перезаписи пути
// Возвращает 1 если путь не изменился, 0 - если изменился, 
// Если размер строки нового пути превышает STR_LEN - путь не изменяется
int choose_dir(int n, struct dirent **list, char* path);

// Функция отрисовки окна
void draw_win(WINDOW *win, struct wnd_set *set, int limit);

// Функция для вывода на экран предупреждения ACCESS DENIED
void warn_win(WINDOW *win);

// Функция навигации
// Возвращает 1 при нажатии клавиши F12(выход из приложения),
// -1 при ошибке, 0 при нажатии Tab(переключение между окнами)
int navigation(WINDOW *win, struct wnd_set *set, int limit);

#endif