#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>
#include <unistd.h>

#define STR_SIZE 10
#define SHM_NAME "/shmemory"
#define SEM_NAME "/semaphore"

#define ERROR_FUNC(str, status, ret) if(ret == status) { \
                                        perror(str); \
                                        munmap(str, STR_SIZE); \
                                        shm_unlink(SHM_NAME); \
                                        sem_close(sem); \
                                        sem_unlink(SEM_NAME); \
                                        close(fd); \
                                        exit(EXIT_FAILURE); \
                                    }

sem_t *sem;

int main(){
    int fd;
    char rcv_str[STR_SIZE] = { 0 };
    char *str;
    int ret = 0;

    fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0644);
    if(fd == -1){
        perror("shm open"); 
        exit(EXIT_FAILURE); 
    }

    sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 1);
    if(sem == NULL){
        close(fd);
        perror("sem open"); 
        exit(EXIT_FAILURE); 
    }

    ret = ftruncate(fd, STR_SIZE);
    if(ret == -1){
        sem_close(sem);
        close(fd);
        perror("ftruncate"); 
        exit(EXIT_FAILURE); 
    }

    str = mmap(NULL, sizeof(*str), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(str == NULL){
        sem_close(sem);
        close(fd);
        perror("mmap"); 
        exit(EXIT_FAILURE); 
    }

    ret = sem_wait(sem);
    ERROR_FUNC("sem 1 wait", -1, ret);
    strcpy(str, "Hello!");
    ret = sem_post(sem);
    ERROR_FUNC("sem 1 post", -1, ret);

    while(strlen(rcv_str) == 0 || strcmp(rcv_str, "Hello!") == 0){
        sem_wait(sem);
        ERROR_FUNC("sem 2 wait", -1, ret);
        strcpy(rcv_str, str);
        sem_post(sem);
        ERROR_FUNC("sem 2 wait", -1, ret);
    }

    printf("Server received: %s\n", rcv_str);

    munmap(str, STR_SIZE);
    shm_unlink(SHM_NAME);
    sem_close(sem);
    sem_unlink(SEM_NAME);
    close(fd);

    exit(EXIT_SUCCESS);
}
