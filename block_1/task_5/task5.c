#include <stdio.h>
#include <string.h>
#include <locale.h>

#define ARR_SIZE 100 // Размер справочника
#define STR_SIZE 10 // Размер полей справочника
#define DEL_SIZE 4 // Размер строки для ввода при удалении записи
#define FUNC_SIZE 2 // Размер строки для ввода номера операции

struct abonent {
    char name[STR_SIZE];
    char second_name[STR_SIZE];
    char tel[STR_SIZE];
};

// Функция ввода строк
void input_string(char *str, int size){
    fgets(str, size, stdin);
    // Если количество введенных пользователем символов больше size
    if(!strchr(str, '\n'))
        // Очищаем stdin
        while(getchar() != '\n');
    else
        // Удаляем символ начала строки из str
        str[strcspn(str, "\n")] = 0;
}

// Функция добавления абонента
void add_abonent(unsigned char* count, struct abonent* arr){
    if(*count == 0){
        printf("\nСправочник переполнен\n");
        return;
    }
    for(int i = 0; i < ARR_SIZE; i++){
        if(arr[i].name[0] == 0 && arr[i].second_name[0] == 0 && arr[i].tel[0] == 0){
            printf("\nВведите имя : ");
            input_string(arr[i].name, STR_SIZE);
            printf("Введите фамилию: ");
            input_string(arr[i].second_name, STR_SIZE);
            printf("Введите номер телефона: ");
            input_string(arr[i].tel, STR_SIZE);
            if(arr[i].name[0] == 0 && arr[i].second_name[0] == 0 && arr[i].tel[0] == 0){
                printf("\nВсе поля пусты, запись не будет добавлена\n");
                break;
            }
            printf("\nЗапись добавлена\n");
            *count -= 1;
            break;
        }
    }
}

// Функция удаления абонента
void delete_abonent(unsigned char* count, struct abonent* arr){
    unsigned short num = 0; // Номер записи для удаления
    char buf[DEL_SIZE]; // Строка для пользовательского ввода
    if (*count == ARR_SIZE){
        printf("\nСправочник пуст\n");
        return;
    }
    printf("\nВведите номер записи, которую вы хотите удалить: ");
    input_string(buf, 4);
    sscanf(buf, "%hu", &num);
    num--;
    if(num >= ARR_SIZE){
        printf("\nОшибка: Неверный номер\n");
        return;
    }
    if(arr[num].name[0] == 0 && arr[num].second_name[0] == 0 && arr[num].tel[0] == 0){
        printf("\nЗапись пуста\n");
        return;
    }
    memset(&arr[num], 0, sizeof(struct abonent));
    printf("\nЗапись удалена\n");
    *count += 1;
}


// Функция поиска абонента
void find_abonent(unsigned char* count, struct abonent* arr){
    char search[STR_SIZE]; // Строка для поиска
    unsigned char search_count = 0; // Количество найденных записей
    if (*count == ARR_SIZE){
        printf("\nСправочник пуст\n");
        return;
    }
    search_count = 0;
    printf("\nВведите фамилию: ");
    input_string(search, STR_SIZE);
    for(int i = 0; i < ARR_SIZE; i++){
        if(arr[i].name[0] == 0 && arr[i].second_name[0] == 0 && arr[i].tel[0] == 0){
            continue;
        }
        if(strcmp(arr[i].second_name, search) == 0){
            printf("\nАбонент #%d:\n", i + 1);
            printf("Имя: %s\n", arr[i].name);
            printf("Фамилия: %s\n", arr[i].second_name);
            printf("Телефон: %s\n", arr[i].tel);
            search_count++;
        }
    }
    if(search_count == 0)
        printf("\nНичего не найдено\n");
}


// Функция вывода всех записей
void list_all(unsigned char* count, struct abonent* arr){
    if (*count == ARR_SIZE){
        printf("\nСправочник пуст\n");
        return;
    }
    for(int i = 0; i < ARR_SIZE; i++){
        if(arr[i].name[0] == 0 && arr[i].second_name[0] == 0 && arr[i].tel[0] == 0){
            continue;
        }
        else{
            printf("\nАбонент #%d:\n", i + 1);
            printf("Имя: %s\n", arr[i].name);
            printf("Фамилия: %s\n", arr[i].second_name);
            printf("Телефон: %s\n", arr[i].tel);
        }
    }
}

int main(void){
    setlocale(LC_ALL, "Rus");
    char flag = 0; // Флаг для выхода из цикла
    char func_num = 0; // Номер операции
    struct abonent arr[ARR_SIZE] = { 0 }; // Справочник
    unsigned char count = ARR_SIZE; // Количество свободных полей
    char buf[FUNC_SIZE]; // Строка для пользовательского ввода

    // Интерфейс
    printf("Максимальное число записей в справочнике - %d\n", ARR_SIZE);
    printf("Операции:\n1 - Добавить абонента\n");
    printf("2 - Удалить абонента\n");
    printf("3 - Поиск абонента по фамилии\n");
    printf("4 - Вывод всех записей\n");
    printf("5 - Выход\n");
    while(1){
        if(flag == 1)
            break;
        printf("\nВведите номер операции: ");
        input_string(buf, 2);
        sscanf(buf, "%hhd", &func_num);
        switch(func_num)
        {
            case 1:
                add_abonent(&count, arr);
                break;
            case 2:
                delete_abonent(&count, arr);
                break;
            case 3:
                find_abonent(&count, arr);
                break;
            case 4:
                list_all(&count, arr);
                break;
            case 5:
                flag = 1;
                break;
            default:
                printf("\nОшибка: неверный номер операции\n");
        }
    }
    return 0;
}