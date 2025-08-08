#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <unistd.h>

#define STR_SIZE 10
#define SHM_NAME "/shmemory"
#define SEM_NAME "/semaphore"

#define ERROR_FUNC(str, status, ret) if(ret == status) { \
                                        perror(str); \
                                        munmap(str, STR_SIZE); \
                                        sem_close(sem); \
                                        close(fd); \
                                        exit(EXIT_FAILURE); \
                                    }

sem_t *sem;

int main(){
    int fd;
    char rcv_str[STR_SIZE] = { 0 };
    char *str;
    int ret = 0;

    while(1){
        fd = shm_open(SHM_NAME, O_RDWR, 0644);
        if(fd == -1){
            if(errno == ENOENT)
                continue;
            perror("shm open");
            exit(EXIT_FAILURE);
        }
        break;
    }

    while(1){
        sem = sem_open(SEM_NAME, 0);
        if(sem == NULL){
            if(errno == ENOENT)
                continue;
            perror("sem open");
            close(fd);
            exit(EXIT_FAILURE);
        }
        break;
    }

    ret = ftruncate(fd, STR_SIZE);
    if(ret == -1){
        sem_close(sem);
        close(fd); 
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    str = mmap(NULL, STR_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(ret == -1){
        sem_close(sem);
        close(fd); 
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    while(strlen(rcv_str) == 0){
        ret = sem_wait(sem);
        ERROR_FUNC("sem 1 wait", -1, ret);
        strcpy(rcv_str, str);
        ret = sem_post(sem);
        ERROR_FUNC("sem 1 post", -1, ret);
    }

    printf("Client received: %s\n", rcv_str);

    ret = sem_wait(sem);
    ERROR_FUNC("sem 2 wait", -1, ret);
    strcpy(str, "Hi!");
    ret = sem_post(sem);
    ERROR_FUNC("sem 2 post", -1, ret);

    munmap(str, STR_SIZE);
    sem_close(sem);
    close(fd);
    
    exit(EXIT_SUCCESS);

}