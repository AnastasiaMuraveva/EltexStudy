#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

#define THREADS_NUM 5
#define N 18000000

pthread_mutex_t mutex;

void *calc(void *arg){
    long *num = (long *) arg;
    int err = 0;
    long count = N / THREADS_NUM;
    for(int i = 0; i < count; i++){
        err = pthread_mutex_lock(&mutex);
        if(err != 0){
            printf("Lock mutex #%d\n", err);
            pthread_exit(NULL);
        }
        *num = *num + 1;
        err = pthread_mutex_unlock(&mutex);
        if(err != 0){
            printf("Unock mutex #%d\n", err);
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
}

int main(){
    pthread_t arr[THREADS_NUM];
    long num = 0;
    int err = 0;

    err = pthread_mutex_init(&mutex, NULL);
    if(err != 0){
        printf("Init mutex error #%d\n", err);
        exit(err);
    }

    for(int i = 0; i < THREADS_NUM; i++){
        err = pthread_create(&arr[i], NULL, calc, (void*) &num);
        if(err != 0){
            printf("Create error #%d\n", err);
            for(int j = 0; j < i; j++){
                pthread_join(arr[j], NULL);
            }
            exit(err);
        }
    }

    for(int i = 0; i < THREADS_NUM; i++){
        err = pthread_join(arr[i], NULL);
        if(err != 0){
            printf("Join error #%d\n", err);
        }
    }

    printf("Final result = %ld\n", num);

    err = pthread_mutex_destroy(&mutex);
    if(err == EBUSY){
        pthread_mutex_unlock(&mutex);
        pthread_mutex_destroy(&mutex);
    }
    else if(err != 0){
        printf("Destroy mutex error #%d\n", err);
    }
    exit(err);
}