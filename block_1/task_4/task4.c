#include <stdio.h>
#include <string.h>
#include <locale.h>

#define ARR_SIZE 100
#define STR_SIZE 10

struct abonent {
    char name[STR_SIZE];
    char second_name[STR_SIZE];
    char tel[STR_SIZE];
};

// Функция ввода строк
void input_string(char *str){
    fgets(str, STR_SIZE, stdin);
    // Если количество введенных пользователем символов больше STR_SIZE
    if(!strchr(str, '\n'))
        // Очищаем stdin
        while(getchar() != '\n');
    else
        // Удаляем символ начала строки из str
        str[strcspn(str, "\n")] = 0;
}

int main(void){
    setlocale(LC_ALL, "Rus");
    char flag = 0; // Флаг для выхода из цикла
    char func_num = 0; // Номер операции
    struct abonent arr[ARR_SIZE] = { 0 }; // Справочник
    unsigned char count = ARR_SIZE; // Количество свободных полей
    unsigned char num = 0; // Номер записи для удаления
    char search[STR_SIZE]; // Строка для поиска
    unsigned char search_count = 0; // Количество найденных записей

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
        scanf("%hhd", &func_num);
        switch(func_num)
        {
            case 1:
                if(count == 0){
                    printf("\nСправочник переполнен\n");
                    break;
                }
                getchar();
                for(int i = 0; i < ARR_SIZE; i++){
                    if(arr[i].name[0] == 0 && arr[i].second_name[0] == 0 && arr[i].tel[0] == 0){
                        printf("\nВведите имя : ");
                        input_string(arr[i].name);
                        printf("Введите фамилию: ");
                        input_string(arr[i].second_name);
                        printf("Введите номер телефона: ");
                        input_string(arr[i].tel);
                        if(arr[i].name[0] == 0 && arr[i].second_name[0] == 0 && arr[i].tel[0] == 0){
                            printf("\nВсе поля пусты, запись не будет добавлена\n");
                            break;
                        }
                        printf("\nЗапись добавлена\n");
                        count--;
                        break;
                    }
                }
                break;
            case 2:
                if (count == ARR_SIZE){
                    printf("\nСправочник пуст\n");
                    break;
                }
                printf("\nВведите номер записи, которую вы хотите удалить: ");
                scanf("%hhu", &num);
                num--;
                if(num >= ARR_SIZE){
                    printf("\nОшибка: Неверный номер\n");
                    break;
                }
                if(arr[num].name[0] == 0 && arr[num].second_name[0] == 0 && arr[num].tel[0] == 0){
                    printf("\nЗапись пуста\n");
                    break;
                }
                memset(&arr[num], 0, sizeof(struct abonent));
                printf("\nЗапись удалена\n");
                count++;
                break;
            case 3:
                if (count == ARR_SIZE){
                    printf("\nСправочник пуст\n");
                    break;
                }
                getchar();
                search_count = 0;
                printf("\nВведите фамилию: ");
                input_string(search);
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
                break;
            case 4:
                if (count == ARR_SIZE){
                    printf("\nСправочник пуст\n");
                    break;
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