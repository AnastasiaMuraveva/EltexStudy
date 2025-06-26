#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

// Функция вывода квадратной матрицы по заданному N
void print_matrix(int N){
    int num = 1;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%d\t", num);
            num++;
        }
        printf("\n");
    }
}

// Функция вывода массива в обратном порядке
void reverse_array(int *arr, int size) {
    int temp;
    for (int i = 0; i < size / 2; i++) {
        temp = arr[i];
        arr[i] = arr[size - 1 - i];
        arr[size - 1 - i] = temp;
    }
}

// функция заполнения треугольной матрицы
void triangle_matrix(int **m, int N){
    int count = 1;
    for(int i = 0; i < N; i++){
        for(int j = N - 1; j >= N - count; j--)
            m[i][j] = 1;
        count++;
    }
}

// Функция заполнения квадратной матрицы "улиткой"
void spiral_matrix(int **m, int N) {
    int num = 1;
    int count = N - 1;
    for (int i = 0; i < N / 2; i++) {
        //Проход вправо
        for (int j = i; j < count; j++) {
            m[i][j] = num;
            num++;
        }
        //Проход вниз
        for (int j = i; j < count; j++) {
            m[j][count] = num;
            num++;
        }
        // Проход влево
        for (int j = count; j > i; j--) {
            m[count][j] = num;
            num++;
        }
        // Проход вверх
        for (int j = count; j > i; j--) {
            m[j][i] = num;
            num++;
        }
        count--;  
    }
    // Если N нечетное - заполняем середину матрицы
    if (N % 2 != 0)
        m[N / 2][N / 2] = N*N;
}

int main(){    	
    setlocale(LC_ALL, "Rus");
    int N = 0; // Размер массива или матрицы
    char flag = 0; // Флаг для выхода из цикла
    char func_num = 0; // Номер операции
    int *arr; // Массив для 2 операции
    int **m; // Матрица для 3-4 операций

    // Интерфейс
    printf("1 - Вывод квадратной матрицы от 1 до N^2\n");
    printf("2 - Вывод обратного массива размера N\n");
    printf("3 - Заполнение верхнего треугольника матрицы размера N нулями, нижнего - единицами\n");
    printf("4 - Заполнение матрицы ""улиткой"" от 1 до N^2\n");
    printf("5 - Выход\n");
    while(1){
        if(flag == 1)
            break;
        printf("Введите номер операции: ");
        scanf("%hhd", &func_num);
        switch(func_num)
        {
            case 1:
                printf("Введите N: ");
                scanf("%d", &N);
                if(N <= 0){
                    printf("Ошибка: N должно быть положительным числом больше нуля");
                    break;
                }
                printf("Результат:\n");
                print_matrix(N);
                break;
            case 2:
                printf("Введите N: ");
                scanf("%d", &N);
                if(N <= 0){
                    printf("Ошибка: N должно быть положительным числом больше нуля");
                    break;
                }
                // Выделение памяти под массив
                arr = calloc(N, sizeof(int));
                for(int i = 0; i < N; i++){
                    printf("Введите %d элемент массива: ", i + 1);
                    scanf("%d", &arr[i]);
                }
                printf("Результат:\n");
                reverse_array(arr, N);
                for(int i = 0; i < N; i++)
                    printf("%d ", arr[i]);
                printf("\n");
                // Освобождение памяти
                free(arr);
                break;
            case 3:
                printf("Введите N: ");
                scanf("%d", &N);
                if(N <= 0){
                    printf("Ошибка: N должно быть положительным числом больше нуля");
                    break;
                }
                // Выделение памяти под матрицу
                m = calloc(N, sizeof(int*));
                for(int i = 0; i < N; i++)
                    m[i] = calloc(N, sizeof(int));
                triangle_matrix(m, N);
                //Вывод матрицы
                printf("Результат:\n");
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++)
                        printf("%d\t", m[i][j]);
                    printf("\n");
                }
                // Освобождение памяти
                for (int i = 0; i < N; i++)
                    free(m[i]);
                free(m);
                break;
            case 4:
                printf("Введите N: ");
                scanf("%d", &N);
                if(N <= 0){
                    printf("Ошибка: N должно быть положительным числом больше нуля");
                    break;
                }
                // Выделение памяти под матрицу
                m = calloc(N, sizeof(int*));
                for(int i = 0; i < N; i++)
                    m[i] = calloc(N, sizeof(int));
                spiral_matrix(m, N);
                //Вывод матрицы
                printf("Результат:\n");
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++)
                        printf("%d\t", m[i][j]);
                    printf("\n");
                }
                // Освобождение памяти
                for (int i = 0; i < N; i++)
                    free(m[i]);
                free(m);
                break;
            case 5:
                flag = 1;
                break;
            default:
                printf("Ошибка: неверный номер операции\n");
        }
    }
    return 0;
}