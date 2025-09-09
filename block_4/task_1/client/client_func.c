#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Функция ввода строк
int input_string(char *str, int size){
    if (fgets(str, size, stdin) == NULL){
        perror("\nОшибка чтения строки\n");
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