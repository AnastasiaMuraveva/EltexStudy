#include <curses.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <signal.h>
#include "file_system_commands.h"

// F12 - выход из приложения
// Tab - переключение окон
// Клавиши вверх/вниз - навигация
// Enter - открыть каталог

int main(){
    int ret = 0;

    WINDOW * wnd1, *wnd2;
    WINDOW * subwnd1, *subwnd2;
    struct wnd_info wnd1_info = {50, 90, 2, 4};
    struct wnd_info wnd2_info = {50, 90, 2, 100};
    struct wnd_info subwnd_info = {48, 88, 1, 1};
    struct wnd_set wnd1_set = {0, 0, "/home", 0, NULL};
    struct wnd_set wnd2_set = {0, 0, "/home", 0, NULL};

    wnd1_set.count = scandir(wnd1_set.path, &wnd1_set.list, 0, alphasort);
    if(wnd1_set.count < 0){
        perror("Scandir error:");
        exit(EXIT_FAILURE);
    }
    wnd2_set.count = scandir(wnd2_set.path, &wnd2_set.list, 0, alphasort);
    if(wnd1_set.count < 0){
        perror("Scandir error:");
        exit(EXIT_FAILURE);
    }

    initscr();
    signal(SIGWINCH, sig_winch);
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    curs_set(0);
    start_color();
    refresh();

    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    wnd1 = newwin(wnd1_info.y, wnd1_info.x, wnd1_info.y_offset, wnd1_info.x_offset);
    box(wnd1, '|', '-');
    wrefresh(wnd1);
    subwnd1 = derwin(wnd1, subwnd_info.y, subwnd_info.x, subwnd_info.y_offset, subwnd_info.x_offset);
    keypad(subwnd1, TRUE);
    wattron(subwnd1, COLOR_PAIR(1));
    draw_win(subwnd1, &wnd1_set, subwnd_info.y);

    wnd2 = newwin(wnd2_info.y, wnd2_info.x, wnd2_info.y_offset, wnd2_info.x_offset);
    box(wnd2, '|', '-');
    wrefresh(wnd2);
    subwnd2 = derwin(wnd2, subwnd_info.y, subwnd_info.x, subwnd_info.y_offset, subwnd_info.x_offset);
    keypad(subwnd2, TRUE);
    wattron(subwnd2, COLOR_PAIR(1));
    draw_win(subwnd2, &wnd2_set, subwnd_info.y);

    while(1){
        ret = navigation(subwnd1, &wnd1_set, subwnd_info.y);
        if (ret == 1)
            break;
        if (ret == -1){
            perror("Navigation error:");
            exit(EXIT_FAILURE);
        }
        ret = navigation(subwnd2, &wnd2_set, subwnd_info.y);
        if (ret == 1)
            break;
        if (ret == -1){
            perror("Navigation error:");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < wnd1_set.count; i++) {
        free(wnd1_set.list[i]);
    }
    for(int i = 0; i < wnd2_set.count; i++) {
        free(wnd2_set.list[i]);
    }
    free(wnd1_set.list);
    free(wnd2_set.list);

    delwin(subwnd1);
    delwin(subwnd2);
    delwin(wnd1);
    delwin(wnd2);
    endwin();
    exit(EXIT_SUCCESS);
}