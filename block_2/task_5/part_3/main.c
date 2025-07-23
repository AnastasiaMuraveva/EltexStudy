#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <sys/wait.h>
#include "operations.h"

int main(){
    setlocale(LC_ALL, "Rus");
    pid_t child_pid;
    int status;
    // Строка для пользовательского ввода
    char str[STR_SIZE];
    // Аргументы программы, в т.ч. название
    char **argv = NULL;
    // Количество аргументов
    int argc = 0;

    printf("Название программы должно содержать полный путь до нее\n");
    while(1){
        printf("Введите команду: ");
        input_string(str);
        if(strcmp(str, "exit") == 0)
            break;
        child_pid = fork();
        if(child_pid == -1){
            perror("Ошибка fork()");
            exit(EXIT_FAILURE);
        }
        if(child_pid == 0){
            if(parser(&argv, &argc, str) == -1){
                printf("Ошибка: пустое поле\n");
                exit(EXIT_FAILURE);
            }
            execv(argv[0], argv);
            // Если execv вернула ошибку
            perror("Ошибка execv()");
            // Освобождаем память только в случае ошибки
            // В случае успешного выполнения execv освободить память мы не имеем возможности
            // Она освобождается автоматически после запуска execv
            free_all(&argv, &argc);
            exit(EXIT_FAILURE);
        }
        else{
            if(wait(&status) == -1){
                perror("Ошибка wait()");
                exit(EXIT_FAILURE);
            }
        }
    }
    exit(EXIT_SUCCESS);
}