#ifndef __OPERATIONS_H__
#define __OPERATIONS_H__

#define STR_SIZE 10 // Размер полей справочника
#define DEL_SIZE 4 // Размер строки для ввода при удалении записи
#define FUNC_SIZE 2 // Размер строки для ввода номера операции

// Коды ошибок
enum errors{
    MEM_ERR = -1,
    STR_ERR = -2
};

// Запись справочника
struct abonent {
    char name[STR_SIZE];
    char second_name[STR_SIZE];
    char tel[STR_SIZE];
};

// Список
struct list{
    struct abonent data;
    struct list *next;
    struct list *prev;
};

// Функция ввода строк
int input_string(char *str, int size);

// Функция добавления абонента
int add_abonent(struct list **head, struct list **tail);

// Функция удаления абонента
int delete_abonent(struct list **head, struct list **tail);

// Функция поиска абонента
int find_abonent(struct list **head);

// Функция вывода всех записей
void list_all(struct list **head);

// Функция освобождения памяти
void free_all(struct list **head);

#endif