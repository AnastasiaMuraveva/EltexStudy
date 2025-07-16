#include <curses.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "file_system_commands.h"

void sig_winch(int signo)
{
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
}

// Функция перезаписи текущего пути на предыдущий в иерархии
void prev_dir(char *path){
    char *find = strrchr(path, '/');
    if(find == path){
        strcpy(path, "/");
        return;
    }
    memset(find, 0, strlen(find));
}

// Функция перезаписи пути
// Возвращает 1 если путь не изменился, 0 - если изменился, 
// Если размер строки нового пути превышает STR_LEN - путь не изменяется
int choose_dir(int n, struct dirent **list, char* path){
    if(strcmp(list[n]->d_name, ".") == 0 || (list[n]->d_type != 4 && list[n]->d_type != 10))
        return 1;
    if(strcmp(list[n]->d_name, "..") == 0){
        prev_dir(path);
    }
    else{
        if((strlen(path) + strlen(list[n]->d_name) + 2) > STR_LEN)
            return 0;
        strcat(path, "/");
        strcat(path, list[n]->d_name);
    }
    return 0;
}

// Функция отрисовки окна
void draw_win(WINDOW *win, struct wnd_set *set, int limit){
    werase(win);
    for(int i = set->head; i < set->count && i < (limit + set->head); i++){
        if(set->cur == i)
            wattron(win, A_REVERSE);
        wprintw(win, "%s\n", set->list[i]->d_name);
        if(set->cur == i)
            wattroff(win, A_REVERSE);
    }
    wrefresh(win);
}

// Функция для вывода на экран предупреждения ACCESS DENIED
void warn_win(WINDOW *win){
    WINDOW *subwin;
    subwin = derwin(win, 1, 20, 0, 0);
    keypad(subwin, TRUE);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    wattron(subwin, COLOR_PAIR(2));
    wprintw(subwin, "ACCESS DENIED!");
    wrefresh(subwin);
    delwin(subwin);
    wrefresh(win);
}

// Функция навигации
// Возвращает 1 при нажатии клавиши F12(выход из приложения),
// -1 при ошибке, 0 при нажатии Tab(переключение между окнами)
int navigation(WINDOW *win, struct wnd_set *set, int limit){
    int user_key = 0;
    int check = 0;
    do{
        user_key = wgetch(win);
        switch(user_key)
        {
            case KEY_DOWN:
                if(set->cur < set->count - 1){
                    set->cur++;
                    if(set->cur >= limit)
                        set->head++;
                }
                draw_win(win, set, limit);
                break;
            case KEY_UP:
                if(set->cur > 0){
                    set->cur--;
                    if(set->cur == set->head && set->head != 0)
                        set->head--;
                }
                draw_win(win, set, limit);
                break;
            case 10:
                check = choose_dir(set->cur, set->list, set->path);
                for(int i = 0; i < set->count; i++) {
                    free(set->list[i]);
                }
                free(set->list);
                set->list = NULL;
                set->count = scandir(set->path, &set->list, 0, alphasort);
                if(set->count < 0){
                    if(errno = 13){
                        warn_win(win);
                        prev_dir(set->path);
                        set->count = scandir(set->path, &set->list, 0, alphasort);
                        break;
                    }
                    return -1;
                }
                if (check == 0)
                    set->cur = 0;
                draw_win(win, set, limit);
                break;
            case KEY_F(12):
                return 1;
            default:
                break;
        }
    } while(user_key != 0x9);
    return 0;
}