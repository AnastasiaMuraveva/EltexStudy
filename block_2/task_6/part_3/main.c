#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include "threads_jobs.h"

int main(){
    pthread_t customers_t[CUSTOMERS_NUM];
    pthread_t supplier_t;
    int ind[CUSTOMERS_NUM];
    int err = 0;
    int ret = 0;

    srand(time(NULL));

    for(int i = 0; i < SHOPS_NUM; i++){
        shops_stocks[i] = rand() % (MAX_SHOP_STOCK - MIN_SHOP_STOCK) + MIN_SHOP_STOCK;
        printf("Shop #%d initial stock = %d\n", i + 1, shops_stocks[i]);
        err = pthread_mutex_init(&mutex[i], NULL);
        if(err != 0){
            printf("Mutex init error #%d, shop #%d\n", err, i + 1);
            for(int j = 0; j < i; j++){
                pthread_mutex_destroy(&mutex[j]);
            }
            exit(err);
        }
    }
    printf("\n");

    for(int i = 0; i < CUSTOMERS_NUM; i++){
        ind[i] = i;
        err = pthread_create(&customers_t[i], NULL, customer, (void *) &ind[i]);
        if(err != 0){
            printf("Create error #%d, customer #%d\n", err, i + 1);
            for(int j = 0; j < i; j++){
                pthread_cancel(customers_t[j]);
                pthread_join(customers_t[j], NULL);
            }
            exit(err);
        }
    }
    err = pthread_create(&supplier_t, NULL, supplier, NULL);
    if(err != 0){
        printf("Create error #%d, supplier\n", err);
        for(int i = 0; i < CUSTOMERS_NUM; i++){
            pthread_cancel(customers_t[i]);
            pthread_join(customers_t[i], NULL);
        }
        exit(err);
    }

    for(int i = 0; i < CUSTOMERS_NUM; i++){
        err = pthread_join(customers_t[i], NULL);
        if(err != 0){
            ret = err;
            printf("Join error #%d, customer #%d\n", err, i + 1);
        }
    }
    err = pthread_join(supplier_t, NULL);
    if(err != 0){
        ret = err;
        printf("Join error #%d, supplier\n", err);
    }

    for(int i = 0; i < SHOPS_NUM; i++){
        err = pthread_mutex_destroy(&mutex[i]);
        if(err == EBUSY){
            pthread_mutex_unlock(&mutex[i]);
            pthread_mutex_destroy(&mutex[i]);
        }
        else if(err != 0){
            ret = err;
            printf("Mutex destroy error #%d, shop #%d\n", err, i + 1);
        }
    }

    exit(ret);
}