#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define N 5

void *print_index(void *arg){
    int *ind = (int*) arg;
    printf("Thread #%d\n", *ind);
    pthread_exit(NULL);
}

int main(){
    int ind[N] = { 0 };
    int err = 0;
    pthread_t arr[N];

    for(int i = 0; i < N; i++){
        ind[i] = i;
        err = pthread_create(&arr[i], NULL, print_index, (void*) &ind[i]);
        if(err != 0){
            printf("Create error thread #%d\n", i);
            for(int j = 0; j < i; j++){
                pthread_join(arr[j], NULL);
            }
            exit(err);
        }
    }

    for(int i = 0; i < N; i++){
        err = pthread_join(arr[i], NULL);
        if(err != 0){
            printf("Join error thread #%d\n", i);
        }
    }

    exit(err);
}