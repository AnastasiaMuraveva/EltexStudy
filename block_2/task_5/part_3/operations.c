#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "operations.h"

int input_string(char *str){
    if (fgets(str, STR_SIZE, stdin) == NULL){
        perror("Ошибка чтения строки");
        return -1;
    }
    // Если количество введенных пользователем символов больше size
    if(!strchr(str, '\n'))
        // Очищаем stdin
        while(getchar() != '\n');
    else
        // Удаляем символ начала строки из str
        str[strcspn(str, "\n")] = 0;
    return 0;
}

int parser(char ***argv, int *argc, char *str){
    int len = strlen(str);
    if (len == 0)
        return -1;
    // Подсчет количества аргументов
    for(int i = 0; i < len; i++){
        // Игнорируем лишние пробелы, если они есть
        if(str[i] == ' ' && str[i + 1] != ' ' && str[i + 1] != '\0')
            (*argc)++;
    }
    // Увеличиваем len, чтобы при выделении памяти точно осталось место под \0
    len++;
    (*argc)++;
    // Выделяем на 1 указатель больше argc, 
    // т.к. execv в качестве последнего указателя принимает NULL
    *argv = calloc(*argc + 1, sizeof(char*));
    for(int i = 0; i < *argc; i++){
        (*argv)[i] = calloc(len, sizeof(char));
        for(int j = 0; j < len; j++){
            if(*str == ' ' || *str == '\0'){
                str++;
                break;
            }
            (*argv)[i][j] = *str;
            str++;
        }
    }
    (*argv)[*argc] = NULL;
    return 0;
}

int free_all(char ***argv, int *argc){
    for(int i = 0; i <= *argc; i++){
        free((*argv)[i]);
    }
    free(*argv);
    *argc = 0;
    return 0;
}