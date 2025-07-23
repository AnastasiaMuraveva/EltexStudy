#include <stdio.h>
#include <locale.h>
#include "calc.h"

int main(void){
    setlocale(LC_ALL, "Rus");
    char flag = 0; // Флаг для выхода из цикла
    char func_num = 0; // Номер операции
    int a, b, c;

    // Интерфейс
    printf("Операции:\n1 - Сложение\n");
    printf("2 - Вычитание\n");
    printf("3 - Деление\n");
    printf("4 - Умножение\n");
    printf("5 - Выход\n");
    while(1){
        if(flag == 1)
            break;
        printf("\nВведите номер операции: ");
        scanf("%hhd", &func_num);
        switch(func_num)
        {
            case 1:
                printf("Введите первое слагаемое: ");
                scanf("%d", &a);
                printf("Введите второе слагаемое: ");
                scanf("%d", &b);
                c = add(a, b);
                printf("Сумма: %d\n", c);
                break;
            case 2:
                printf("Введите уменьшаемое: ");
                scanf("%d", &a);
                printf("Введите вычитаемое: ");
                scanf("%d", &b);
                c = sub(a, b);
                printf("Разность: %d\n", c);
                break;
            case 3:
                printf("Введите первый множитель: ");
                scanf("%d", &a);
                printf("Введите второй множитель: ");
                scanf("%d", &b);
                c = mul(a, b);
                printf("Произведение: %d\n", c);
                break;
            case 4:
                printf("Введите делимое: ");
                scanf("%d", &a);
                printf("Введите делитель: ");
                scanf("%d", &b);
                c = div(a, b);
                printf("Частное: %d\n", c);
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