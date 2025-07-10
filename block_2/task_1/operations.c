#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "operations.h"

// Функция ввода строк
int input_string(char *str, int size){
    if (fgets(str, size, stdin) == NULL){
        perror("\nОшибка чтения строки\n");
        return STR_ERR;
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

// Функция добавления абонента
int add_abonent(struct list **head, struct list **tail){
    struct list *tmp = calloc(1, sizeof(struct list));
    if(tmp == NULL){
        perror("\nОшибка выделения памяти\n");
        return MEM_ERR;
    }
    printf("\nВведите имя : ");
    if(input_string(tmp->data.name, STR_SIZE) < 0)
        return STR_ERR;
    printf("Введите фамилию: ");
    if(input_string(tmp->data.second_name, STR_SIZE) < 0)
        return STR_ERR;
    printf("Введите номер телефона: ");
    if(input_string(tmp->data.tel, STR_SIZE) < 0)
        return STR_ERR;
    if(tmp->data.name[0] == 0 && tmp->data.second_name[0] == 0 && tmp->data.tel[0] == 0){
        printf("\nВсе поля пусты, запись не будет добавлена\n");
        free(tmp);
        return 0;
    }
    // Если в справочнике нет записей
    if(*head == NULL){
        *head = tmp;
        *tail = tmp;
    }
    // Если в справочнике одна запись
    else if((*head)->next == NULL){
        tmp->prev = *head;
        (*head)->next = tmp;
        *tail = tmp;
    }
    // Если в справочнике > 1 записи
    else{
        tmp->prev = *tail;
        (*tail)->next = tmp;
        *tail = tmp;
    }
    printf("\nЗапись добавлена\n");
    return 0;
}


// Функция удаления абонента
int delete_abonent(struct list **head, struct list **tail){
    struct list *tmp = *head;
    unsigned int num = 0; // Номер записи для удаления
    char buf[DEL_SIZE]; // Строка для пользовательского ввода
    if (*head == NULL){
        printf("\nСправочник пуст\n");
        return 0;
    }
    printf("\nВведите номер записи, которую вы хотите удалить: ");
    if(input_string(buf, 4) < 0)
        return STR_ERR;
    sscanf(buf, "%u", &num);
    while(tmp != NULL && num > 1){
        tmp = tmp->next;
        num--;
    }
    if(tmp == NULL || num < 1){
        printf("\nТакого номера не существует\n");
        return 0;
    }
    // Если удаляем единственный элемент
    if(*head == *tail){
        *head = NULL;
    }
    // Если удаляем первый элемент
    else if(tmp == *head){
        *head = (*head)->next;
        (*head)->prev = NULL;
    }
    // Если удаляем последний элемент
    else if(tmp == *tail){
        *tail = (*tail)->prev;
        (*tail)->next = NULL;
    }
    // Если удаляем элемент в середине
    else{
        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
    }
    free(tmp);
    printf("\nЗапись удалена\n");
    return 0;
}


// Функция поиска абонента
int find_abonent(struct list **head){
    struct list *tmp = *head;
    unsigned int num = 1;
    char search[STR_SIZE]; // Строка для поиска
    unsigned char search_count = 0; // Количество найденных записей
    if (*head == NULL){
        printf("\nСправочник пуст\n");
        return 0;
    }
    search_count = 0;
    printf("\nВведите фамилию: ");
    if(input_string(search, STR_SIZE) < 0)
        return STR_ERR;
    while(tmp != NULL){
        if(strcmp(tmp->data.second_name, search) == 0){
            printf("\nАбонент #%d:\n", num);
            printf("Имя: %s\n", tmp->data.name);
            printf("Фамилия: %s\n", tmp->data.second_name);
            printf("Телефон: %s\n", tmp->data.tel);
            search_count++;
        }
        num++;
        tmp = tmp->next;
    }
    if(search_count == 0)
        printf("\nНичего не найдено\n");
    return 0;
}


// Функция вывода всех записей
void list_all(struct list **head){
    struct list *current = *head;
    int num = 1;
    if(*head == NULL){
        printf("\nСправочник пуст\n");
        return;
    }
    while(current != NULL){
        printf("\nАбонент #%d:\n", num);
        printf("Имя: %s\n", current->data.name);
        printf("Фамилия: %s\n", current->data.second_name);
        printf("Телефон: %s\n", current->data.tel);
        current = current->next;
        num++;
    }
}


// Функция освобождения памяти
void free_all(struct list **head){
    struct list *current;
    if(*head == NULL)
        return;
    while(*head != NULL){
        current = *head;
        *head = (*head)->next;
        free(current);
    }
}