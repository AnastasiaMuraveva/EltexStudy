#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#define STR_SIZE 10

#define ERROR_FUNC(str, status, ret) if(ret == status) { \
                                        perror(str); \
                                        shmdt(str); \
                                        exit(EXIT_FAILURE); \
                                    }

struct msgbuf {
    long mtype; 
    char mtext[STR_SIZE];

}; 

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

    while(1){
        shmid = shmget(key, STR_SIZE, 0644);
        if(shmid == -1){
            if(errno == ENOENT){
                continue;
            }
            perror("shmget error");
            exit(EXIT_FAILURE);
        }
        break;
    }

    key = ftok(sem_path, id);
    ERROR_FUNC("ftok", -1, ret);

    while(1){
       semid = semget(key, 1, 0644);
        if(semid == -1){
            if(errno == ENOENT){
                continue;
            }
            ERROR_FUNC("semget", -1, ret);
        }
        break;
    }

    str = shmat(shmid, NULL, 0);
    if(str == NULL){
        perror("shmat error");
        exit(EXIT_FAILURE);
    }

    while(strlen(rcv_str) == 0){
        ret = semop(semid, &op, 1);
        ERROR_FUNC("sem client 1 wait", -1, ret);
        strcpy(rcv_str, str);
        op.sem_op = 1;
        ret = semop(semid, &op, 1);
        ERROR_FUNC("sem client 1 post", -1, ret);
        op.sem_op = -1;
    }

    printf("Client received: %s\n", rcv_str);

    ret = semop(semid, &op, 1);
    ERROR_FUNC("sem client 2 wait", -1, ret);
    strcpy(str, "Hi!");
    op.sem_op = 1;
    ret = semop(semid, &op, 1);
    ERROR_FUNC("sem client 2 post", -1, ret);

    shmdt(str);
    exit(EXIT_SUCCESS);
}