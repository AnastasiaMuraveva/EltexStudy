#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#define STR_SIZE 10

#define ERROR_FUNC(str, status, ret) if(ret == status) { \
                                        perror(str); \
                                        semctl(semid, 0, IPC_RMID, NULL); \
                                        shmdt(str); \
                                        shmctl(shmid, IPC_RMID, NULL); \
                                        exit(EXIT_FAILURE); \
                                    }

int main(){
    key_t key;
    int shmid;
    char rcv_str[STR_SIZE] = { 0 };
    char *str;
    int id = 112;
    int semid;
    int ret = 0;
    const char shm_path[] = "path.txt";
    const char sem_path[] = "path2.txt";
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = 0;

    key = ftok(shm_path, id);
    if(key == -1){
        perror("ftok error");
        exit(EXIT_FAILURE);
    }

    shmid = shmget(key, STR_SIZE, IPC_CREAT | IPC_EXCL | 0644);
    if(shmid == -1){
        perror("shmget error");
        exit(EXIT_FAILURE);
    }

    str = shmat(shmid, NULL, 0);
    if(str == NULL){
        perror("shmat error");
        shmctl(shmid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }

    key = ftok(sem_path, id);
    if(key == -1){
        perror("ftok 2 error");
        shmdt(str); 
        shmctl(shmid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }

    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0644);
    if(semid == -1){
        perror("semget error");
        shmdt(str); 
        shmctl(shmid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }

    ret = semctl(semid, 0, SETVAL, 1);
    ERROR_FUNC("semctl", -1, ret);

    ret = semop(semid, &op, 1);
    ERROR_FUNC("sem 1 wait", -1, ret);
    strcpy(str, "Hello!");
    op.sem_op = 1;
    ret = semop(semid, &op, 1);
    ERROR_FUNC("sem 1 post", -1, ret);

    while(strlen(rcv_str) == 0 || strcmp(rcv_str, "Hello!") == 0){
        ret = semop(semid, &op, 1);
        ERROR_FUNC("sem 2 wait", -1, ret);
        strcpy(rcv_str, str);
        op.sem_op = 1;
        ret = semop(semid, &op, 1);
        ERROR_FUNC("sem 2 wait", -1, ret);
        op.sem_op = -1;
    }

    printf("Server received: %s\n", rcv_str);

    shmdt(str);
    ret = semctl(semid, 0, IPC_RMID);
    if(ret == -1){
        perror("semctl error");
    }
    ret = shmctl(shmid, IPC_RMID, NULL);
    if(ret == -1){
        perror("shmctl error");
    }
    exit(EXIT_SUCCESS);
}