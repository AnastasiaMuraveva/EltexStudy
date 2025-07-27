#ifndef __THREADS_JOBS_H__
#define __THREADS_JOBS_H__
#include <pthread.h>

// Константы
// Количество покупателей
#define CUSTOMERS_NUM 3
// Количество магазинов
#define SHOPS_NUM 5
// Максимальная потребность покупателя
#define MAX_DEMAND 100000
// Минимальная потребность покупателя
#define MIN_DEMAND 90000
// Размер одной поставки
#define ONE_SUPPLY 5000
// Максимальный начальный запас магазина
#define MAX_SHOP_STOCK 10000
// Минимальный начальный запас магазина
#define MIN_SHOP_STOCK 8000

// Глобальные переменные
// Массив мьютексов для каждого магазина
extern pthread_mutex_t mutex[SHOPS_NUM];
// Запасы магазина
extern int shops_stocks[SHOPS_NUM];
// Потребность покупателей
extern int demand[CUSTOMERS_NUM];

// Функции потоков
// Покупатель
void *customer(void *arg);
// Поставщик
void *supplier();

#endif