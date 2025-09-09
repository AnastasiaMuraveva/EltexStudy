#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "base.h"

int input_string(char *str, int size){
    if (fgets(str, size, stdin) == NULL){
        perror("fgets error");
        return -1;
    }
    // Если количество введенных пользователем символов больше size
    if(!strchr(str, '\n'))
        // Очищаем stdin
        while(getchar() != '\n');
    else
        // Удаляем символ начала строки из str
        str[strcspn(str, "\n")] = 0;
    return 0;
}

int add_driver(struct drivers_list **head, struct drivers_list **tail, int fd, int pid){
    struct drivers_list *tmp = calloc(1, sizeof(struct drivers_list));
    if(tmp == NULL){
        perror("add driver calloc error");
        return -1;
    }

    tmp->data.fd = fd;
    tmp->data.pid = pid;
    tmp->next = NULL;

    // Если список пуст
    if(*head == NULL){
        *head = tmp;
        *tail = tmp;
    }
    else{
        (*tail)->next = tmp;
        *tail = tmp;
    }
    return 0;
}

void free_all(struct drivers_list **head){
    struct drivers_list *current;
    int ret = 0;
    char buf[MSG_SIZE];
    strcpy(buf, "exit");
    if(*head == NULL)
        return;
    while(*head != NULL){
        current = *head;
        *head = (*head)->next;
        ret = write(current->data.fd, buf, MSG_SIZE);
        if (ret == -1) {
            perror("free all write error");
        }
        ret = waitpid(current->data.pid, NULL, 0);
        if (ret == -1) {
            perror("free all waitpid error");
        }
        close(current->data.fd);
        free(current);
    }
}

int get_fd(struct drivers_list **head, int pid){
    struct drivers_list *tmp = *head;
    while(tmp != NULL){
        if(tmp->data.pid == pid){
            return tmp->data.fd;
        }
        tmp = tmp->next;
    }
    return -1;
}

int send_task(int child_fd, int parent_fd, int time){
    int ret = 0;
    char buf[MSG_SIZE];
    sprintf(buf, "%d", time);
    ret = write(child_fd, buf, MSG_SIZE);
    if(ret == -1){
        perror("send task write error");
        return -1;
    }
    memset(buf, 0, MSG_SIZE);
    ret = read(parent_fd, buf, MSG_SIZE);
    if(ret == -1){
        perror("send task read error");
        return -1;
    }
    if(strcmp(buf, "ok") == 0){
        return 0;
    }
    else{
        printf("error: %s\n", buf);
    }
    return 0;
}

int get_status(int child_fd, int parent_fd){
    int ret = 0;
    char buf[MSG_SIZE];
    strcpy(buf, "status");
    ret = write(child_fd, buf, MSG_SIZE);
    if(ret == -1){
        perror("get status write error");
        return -1;
    }
    memset(buf, 0, MSG_SIZE);
    ret = read(parent_fd, buf, MSG_SIZE);
    if(ret == -1){
        perror("get status read error");
        return -1;
    }
    printf("%s\n", buf);
    return 0;
}

int get_all_status(struct drivers_list **head, int parent_fd){
    struct drivers_list *tmp = *head;
    int ret = 0;
    char buf[MSG_SIZE];
    while(tmp != NULL){
        strcpy(buf, "status");
        ret = write(tmp->data.fd, buf, MSG_SIZE);
        if(ret == -1){
            perror("get all status write error");
            return -1;
        }
        memset(buf, 0, MSG_SIZE);
        ret = read(parent_fd, buf, MSG_SIZE);
        if(ret == -1){
            perror("get all status read error");
            return -1;
        }
        printf("driver %d: %s\n", tmp->data.pid, buf);
        tmp = tmp->next;
    }
    return 0;
}