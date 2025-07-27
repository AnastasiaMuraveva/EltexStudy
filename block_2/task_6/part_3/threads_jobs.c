#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "threads_jobs.h"

pthread_mutex_t mutex[SHOPS_NUM];
int shops_stocks[SHOPS_NUM];
int demand[CUSTOMERS_NUM];

void *customer(void *arg){
    int *ind = (int *) arg;
    int err;
    demand[*ind] = rand() % (MAX_DEMAND - MIN_DEMAND) + MIN_DEMAND;
    printf("Customer #%d initial demand = %d\n", *ind + 1, demand[*ind]);
    while(demand[*ind] > 0){
        for(int i = 0; i < SHOPS_NUM && demand[*ind] > 0; i++){
            err = pthread_mutex_trylock(&mutex[i]);
            if (err == EBUSY)
                continue;
            else if(err != 0){
                printf("Mutex trylock error #%d customer #%d\n", err, *ind + 1);
                // Если произошла ошибка зануляем demand покупателя, чтобы корректно завершить поток поставщика
                demand[*ind] = 0;
                pthread_exit(NULL);
            }
            if(shops_stocks[i] == 0){
                err = pthread_mutex_unlock(&mutex[i]);
                if(err != 0){
                    printf("Mutex unlock error #%d customer #%d\n", err, *ind + 1);
                    demand[*ind] = 0;
                    pthread_exit(NULL);
                }
                continue;
            }
            if(shops_stocks[i] <= demand[*ind]){
                demand[*ind] = demand[*ind] - shops_stocks[i];
                shops_stocks[i] = 0;
            }
            else{
                shops_stocks[i] = shops_stocks[i] - demand[*ind];
                demand[*ind] = 0;
            }
            printf("Customer #%d demand = %d\nShop #%d stock = %d\n\n", *ind + 1, demand[*ind], i + 1, shops_stocks[i]);
            err = pthread_mutex_unlock(&mutex[i]);
            if(err != 0){
                printf("Mutex unlock error #%d customer #%d\n", err, *ind + 1);
                demand[*ind] = 0;
                pthread_exit(NULL);
            }
            sleep(2);
        }
    }
    pthread_exit(NULL);
}

void *supplier(){
    int flag = 1;
    int err;
    while(flag){
        for(int i = 0; i < SHOPS_NUM; i++){
            err = pthread_mutex_trylock(&mutex[i]);
            if(err == EBUSY)
                continue;
            else if (err != 0){
                printf("Mutex trylock error #%d supplier \n", err);
                // Если произошла ошибка зануляем demand, чтобы корректно завершить потоки покупателей
                memset(demand, 0, CUSTOMERS_NUM * sizeof(int));
                pthread_exit(NULL);
            }
            shops_stocks[i] = shops_stocks[i] + ONE_SUPPLY;
            printf("Supplier in shop #%d\nShop #%d stock = %d\n\n", i + 1, i + 1, shops_stocks[i]);
            err = pthread_mutex_unlock(&mutex[i]);
            if(err != 0){
                printf("Mutex unlock error #%d supplier\n", err);
                memset(demand, 0, CUSTOMERS_NUM * sizeof(int));
                pthread_exit(NULL);
            }
            sleep(1);
        }
        // Если все потоки-покупатели закончили работу, завершаем поставки
        for(int i = 0; i < CUSTOMERS_NUM; i++){
            if(demand[i] != 0)
                break;
            if(i == CUSTOMERS_NUM - 1)
                flag = 0;
        }

    }
    pthread_exit(NULL);
}