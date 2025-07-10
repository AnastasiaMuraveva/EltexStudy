#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include "operations.h"

int main(void){
    setlocale(LC_ALL, "Rus");
    char flag = 0; // Флаг для выхода из цикла
    char func_num = 0; // Номер операции
    char buf[FUNC_SIZE]; // Строка для пользовательского ввода
    struct list *head = NULL;
    struct list *tail = NULL;

    // Интерфейс
    printf("Операции:\n1 - Добавить абонента\n");
    printf("2 - Удалить абонента\n");
    printf("3 - Поиск абонента по фамилии\n");
    printf("4 - Вывод всех записей\n");
    printf("5 - Выход\n");
    while(1){
        if(flag == 1)
            break;
        printf("\nВведите номер операции: ");
        if(input_string(buf, 2) < 0)
            exit(EXIT_FAILURE);
        sscanf(buf, "%hhd", &func_num);
        switch(func_num)
        {
            case 1:
                if(add_abonent(&head, &tail) < 0)
                    exit(EXIT_FAILURE);
                break;
            case 2:
                if(delete_abonent(&head, &tail) < 0)
                    exit(EXIT_FAILURE);
                break;
            case 3:
                if(find_abonent(&head) < 0)
                    exit(EXIT_FAILURE);
                break;
            case 4:
                list_all(&head);
                break;
            case 5:
                flag = 1;
                free_all(&head);
                break;
            default:
                printf("\nОшибка: неверный номер операции\n");
        }
    }
    return 0;
}