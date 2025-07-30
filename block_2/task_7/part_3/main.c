#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include "operations.h"

int main(){
    setlocale(LC_ALL, "Rus");
    int pipefd[2];
    pid_t child_pid;
    int status;
    // Строка для пользовательского ввода
    char str[STR_SIZE];
    // Аргументы программы, в т.ч. название
    char **argv = NULL;
    // Количество аргументов
    int argc = 0;
    int ret = 0;

    pipe(pipefd);

    printf("Название программы должно содержать полный путь до нее\n");
    printf("(Кроме тех, чей путь прописан в PATH)\n");
    while(1){
        printf("Введите команду: ");
        input_string(str);
        if(strcmp(str, "exit") == 0)
            break;
        child_pid = fork();
        ERROR_FUNC("Ошибка fork 1", -1, child_pid);
        if(child_pid == 0){
            if(parser(&argv, &argc, str) == -1){
                printf("Ошибка: пустое поле\n");
                exit(EXIT_FAILURE);
            }
            if(strchr(str, '|') != NULL){
                ret = dup2(pipefd[1], 1);
                ERROR_FUNC("Ошибка dup2", -1, ret);
            }
            ret = close(pipefd[0]);
            ERROR_FUNC("Ошибка close 1", -1, ret);
            ret = close(pipefd[1]);
            ERROR_FUNC("Ошибка close 2", -1, ret);
            execvp(argv[0], argv);
            // Если execv вернула ошибку
            perror("Ошибка execv()");
            free_all(&argv, &argc);
            exit(EXIT_FAILURE);
        }
        else{
            ret = wait(&status) == -1;
            ERROR_FUNC("Ошибка wait 1", -1, ret);
            if(strchr(str, '|') != NULL && WEXITSTATUS(status) == 0){
                child_pid = fork();
                ERROR_FUNC("Ошибка fork 2", -1, child_pid);
                if(child_pid == 0){
                    ret = dup2(pipefd[0], 0);
                    ERROR_FUNC("Ошибка dup2", -1, ret);
                    ret = close(pipefd[0]);
                    ERROR_FUNC("Ошибка close 3", -1, ret);
                    ret = close(pipefd[1]);
                    ERROR_FUNC("Ошибка close 4", -1, ret);
                    if(add_parser(&argv, &argc, str) == -1){
                        printf("Ошибка: ожидалось выражение после |\n");
                        exit(EXIT_FAILURE);
                    }
                    execvp(argv[0], argv);
                    // Если execv вернула ошибку
                    perror("Ошибка execv()");
                    free_all(&argv, &argc);
                    exit(EXIT_FAILURE);
                    
                }
                else{
                    ret = wait(&status);
                    ERROR_FUNC("Ошибка wait", -1, ret);
                    // После завершения дочерних процессов очищаем канал
                    // Переводим дескриптор в неблокирующий режим
                    ret = fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
                    ERROR_FUNC("Ошибка fcntl 1", -1, ret);
                    // Считываем из дескриптора все, что там осталось
                    while(1){
                        ret = read(pipefd[0], str, STR_SIZE);
                        if(ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
                            break;
                        else if(ret == -1)
                            ERROR_FUNC("Ошибка очистки канала", -1, ret);
                    }
                    // Переводим дескриптор в блокирующий режим
                    ret = fcntl(pipefd[0], F_SETFL, 0);
                    ERROR_FUNC("Ошибка fcntl 2", -1, ret);
                }
            }
        }
    }
    ret = close(pipefd[0]);
    ERROR_FUNC("Ошибка close 5", -1, ret);
    ret = close(pipefd[1]);
    ERROR_FUNC("Ошибка close 6", -1, ret);
    exit(EXIT_SUCCESS);
}