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
    int j = 0;
    if (len == 0)
        return -1;

    // Подсчет количества аргументов
    while(str[j] != '\0' && str[j] != '|'){
        while(str[j] == ' '){
            j++;
        }
        if(str[j] == '|' || str[j] == '\0')
            break;
        while(str[j] != ' ' && str[j] != '|' && str[j] != '\0'){
            j++;
        }
        (*argc)++;
    }
    if(*argc == 0)
        return -1;
    len = j + 1;
    // Удаляем пробелы из начала строки, если они есть
    while(*str == ' ')
        str++;
    // Выделяем на 1 указатель больше argc, 
    // т.к. execv в качестве последнего указателя принимает NULL
    *argv = calloc(*argc + 1, sizeof(char*));
    for(int i = 0; i < *argc; i++){
        (*argv)[i] = calloc(len, sizeof(char));
        for(int j = 0; j < len; j++){
            if(*str == ' '){
                while(*str == ' ')
                    str++;
                break;
            }
            else if(*str == '\0' || *str == '|')
                break;
            (*argv)[i][j] = *str;
            str++;
        }
    }
    (*argv)[*argc] = NULL;
    return 0;
}

int add_parser(char ***argv, int *argc, char *str){
    int len = 1;
    char *begin;

    begin = strchr(str, '|');
    begin++;
    if(strlen(begin) == 0){
        return -1;
    }
    // Удаляем пробелы из начала строки, если они есть
    while(*begin == ' '){
        begin++;
    } 
    str = begin;
    // Подсчет количества аргументов
    while(*begin != '\0'){
        while(*begin == ' '){
            begin++;
            len++;
        }
        if(*begin == '\0')
            break;
        while(*begin != ' ' && *begin != '\0'){
            begin++;
            len++;
        }
        (*argc)++;
    }

    if((*argc) == 0){
        return -1;
    }
    // Выделяем на 1 указатель больше argc, 
    // т.к. execv в качестве последнего указателя принимает NULL
    *argv = calloc(*argc + 1, sizeof(char*));
    for(int i = 0; i < *argc; i++){
        (*argv)[i] = calloc(len, sizeof(char));
        for(int j = 0; j < len; j++){
            if(*str == ' '){
                while(*str == ' ')
                    str++;
                break;
            }
            else if(*str == '\0')
                break;
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
    return 0;
}