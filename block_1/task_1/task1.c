#include <stdio.h>
#include <locale.h>

// Константы
#define int_size 32
#define byte_size 8

// Функция вывода двоичного представления числа
void print_binary(int number) {
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

// Функция подсчета единиц в двоичном представлении 
// целого положительного числа
 char count_ones(int number) {
    // Маска вида 0...0001
    int mask = 1;
    char count = 0;
    while (number > 0) {
        if ((number | mask) == number) 
            count++;
        number >>= 1;
    }
    return count;
}

// Функция замены 3-его байта в целом положительном числе
int replace_byte(int number, int number2){
    char shift = 2;
    // Маска с нулями на третьем байте
    unsigned int mask = 0xff00ffff;
    // Обнуляем третий байт
    number &= mask;
    // Производим замену байта
    number |= (number2 << byte_size * shift);
    return number;
}

int main(){    	
    setlocale(LC_ALL, "Rus");
    char func_num = 0; // Номер операции
    int number = 0; // Число для 1-3 операций
    int number2 = 0; // Второе число для 3 операции
    char flag = 0; // Флаг для выхода из цикла
    int replaced = 0; // Результат 3 операции
    char count = 0; // Результат 2 операции

    // Интерфейс
    printf("1 - Вывод двоичного представления целого положительного или целого отрицательного числа\n");
    printf("2 - Подсчет количества единиц в двоичном представлении целого положительного числа\n");
    printf("3 - Замена 3-его байта в целом положительном числе\n");
    printf("4 - Выход\n");
    while(1){
        if(flag == 1)
            break;
        printf("Введите номер операции: ");
        scanf("%hhd", &func_num);
        switch(func_num)
        {
            case 1:
                printf("Введите число: ");
                scanf("%d", &number);
                printf("Результат: ");
                print_binary(number);
                break;
            case 2:
                printf("Введите число: ");
                scanf("%d", &number);
                if (number >= 0){
                    count = count_ones(number);
                    printf("Результат: %d\n", count);
                }
                else
                    printf("Ошибка: число должно быть положительным\n");
                break;
            case 3:
                printf("Введите первое число: ");  
                scanf("%d", &number);
                if(number < 0){
                    printf("Ошибка: число должно быть положительным\n");
                    break;
                }
                printf("Введите второе число: ");
                scanf("%d", &number2);
                if(number2 < 0){
                    printf("Ошибка: число должно быть положительным\n");
                    break;
                }
                if(number2 > 255){
                    printf("Ошибка: число не должно превышать 8 бит\n");
                    break;
                }
                else{
                    printf("Первое число: ");
                    print_binary(number);
                    printf("Второе число: ");
                    print_binary(number2);
                    replaced = replace_byte(number, number2);
                    printf("Результат: "); 
                    print_binary(replaced);
                }
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