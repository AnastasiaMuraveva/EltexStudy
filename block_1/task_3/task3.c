#include <stdio.h>
#include <string.h>
#include <locale.h>

// Константы
#define int_size 32
#define N 10
#define str_size 30

// Функция вывода двоичного представления числа из ЛР 1
void print_binary(unsigned int number) {
    // Маска вида 10000...0
    unsigned int mask = 0x80000000;
    for (int i = 0; i < int_size; i++) {
        if ((number | mask) == number)
            printf("1");
        else
            printf("0");
        mask >>= 1;
    }
    printf("\n");
}

// Функция замены 3-его байта
int replace_byte(int number, char number2){
    char *ptr = &number;
    ptr += 2;
    *ptr = number2;
    return number;
}

// Функция вывода массива
void print_array(int* arr){
    for(int i = 0; i < N; i++){
        printf("%d " , *arr);
        arr++;
    }
    printf("\n");
}

// Функция поиска подстроки в строке
char* find_string(char* str, char* sub) {
    if (str == NULL || sub == NULL) return NULL;
    int str_len = strlen(str);
    int sub_len = strlen(sub);
    if (sub_len > str_len || str_len == 0 || sub_len == 0) return NULL;
    while(str_len >= sub_len) {
        for (int i = 0; i < sub_len; i++) {
            if (*(str + i) != *(sub + i))
                break;
            if (*(sub + i + 1) == '\0')
                return str;
        }
        str++;
        str_len--;
    }
    return NULL;
}

int main(void){
    setlocale(LC_ALL, "Rus");
    char flag = 0; // Флаг для выхода из цикла
    char func_num = 0; // Номер операции
    unsigned int number = 0; // Первое число для 1 операции
    unsigned char number2 = 0; // Второе число для 1 операции
    unsigned int replaced = 0; // Результат 1 операции
    char str[str_size];  // Строка для 3 операции
    char substr[str_size]; // Подстрока для 3 операции
    char* str_ptr; // Результат 3 операции
    int arr[N]; // Массив для 2 операции
    // Заполняем массив числами от 1 до N
    for(int i = 0; i < N; i++)
        arr[i] = i + 1;

    // Интерфейс
    printf("1 - Замена 3-его байта в целом положительном числе\n");
    printf("2 - Вывод массива из 10 элементов от 1 до 10\n");
    printf("3 - Поиск подстроки в строке\n");
    printf("4 - Выход\n");
    while(1){
        if(flag == 1)
            break;
        printf("Введите номер операции: ");
        scanf("%hhd", &func_num);
        switch(func_num)
        {
            case 1:
                printf("Введите первое число: ");  
                scanf("%u", &number);
                printf("Введите второе число: ");
                scanf("%hhu", &number2);
                printf("Первое число: ");
                print_binary(number);
                printf("Второе число: ");
                print_binary(number2);
                replaced = replace_byte(number, number2);
                printf("Результат: "); 
                print_binary(replaced);
                break;
            case 2:
                printf("Результат: "); 
                print_array(arr);
                break;
            case 3:
                // Удаляем символ начала строки из stdin
                getchar();
                printf("Введите строку: ");
                fgets(str, str_size, stdin);
                // Если количество введенных пользователем символов больше str_size
                if(!strchr(str, '\n'))
                    // Очищаем stdin
                    while(getchar() != '\n');
                else
                    // Удаляем символ начала строки из str
                    str[strcspn(str, "\n")] = 0;
                printf("Введите подстроку: ");
                fgets(substr, str_size, stdin);
                // Если количество введенных пользователем символов больше str_size
                if(!strchr(substr, '\n'))
                    // Очищаем stdin
                    while(getchar() != '\n');
                else
                    // Удаляем символ начала строки из substr
                    substr[strcspn(substr, "\n")] = 0;
                str_ptr = find_string(str, substr);
                if(str_ptr == NULL){
                    printf("Подстрока не найдена\n");
                    break;
                }
                printf("Результат: %s\n", str_ptr);
                break;
            case 4:
                flag = 1;
                break;
            default:
                printf("Ошибка: неверный номер операции\n");
        }
    }
    return 0;
}