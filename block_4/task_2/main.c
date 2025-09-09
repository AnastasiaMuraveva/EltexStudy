#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include "drivers.h"
#include "base.h"

#define ERROR_FUNC(str){free_all(&head); \
                        close(parent_read_fd); \
                        err(EXIT_FAILURE, str);}


int main(){
    struct drivers_list *head = NULL;
    struct drivers_list *tail = NULL;
    char input[INPUT_LEN];
    int ret = 0;
    int is_fd_open = 0;
    int pid;
    int time;
    int parent_read_fd, child_read_fd;
    const char parent_fd_name[] = "fds/parent_read";
    char child_fd_name[PIPE_NAME_LEN];
    int drivers_count = 1;

    ret = mkfifo(parent_fd_name, 0644);
    if(ret == -1 && errno != EEXIST)
        err(EXIT_FAILURE, "mkfifo 1 error");

    printf("operations:\n");
    printf("create_driver\n");
    printf("send_task <pid> <task_timer>\n");
    printf("get_status <pid>\n");
    printf("get_drivers\n");
    printf("exit\n\n");

    while(1)
    {
        printf("Input an operation: ");
        ret = input_string(input, INPUT_LEN);
        if(ret == -1){
            ERROR_FUNC("input string error");
        }
        if(strcmp(input, "create_driver") == 0)
        {
            sprintf(child_fd_name, "fds/child_read_%d", drivers_count);
            ret = mkfifo(child_fd_name, 0644);
            if(ret == -1 && errno != EEXIST)
                ERROR_FUNC("mkfifo 2 error");
            pid = fork();
            if(pid == -1){
                ERROR_FUNC("fork error");
            }
            if(pid == 0){
                parent_read_fd = open(parent_fd_name, O_WRONLY);
                if(parent_read_fd == -1){
                    err(EXIT_FAILURE, "child process open 1 error");;
                }
                child_read_fd = open(child_fd_name, O_RDONLY);
                if(child_read_fd == -1){
                    close(parent_read_fd);
                    err(EXIT_FAILURE, "child process open 2 error");;
                }
                ret = driver_job(child_read_fd, parent_read_fd);
                if(ret == -1){
                    close(parent_read_fd);
                    close(child_read_fd);
                    exit(EXIT_FAILURE);
                }
                close(parent_read_fd);
                close(child_read_fd);
                exit(EXIT_SUCCESS);
            }
            else{
                if(!is_fd_open){
                    parent_read_fd = open(parent_fd_name, O_RDONLY);
                    if(parent_read_fd == -1){
                        ERROR_FUNC("open 1 error");
                    }
                    is_fd_open = 1;
                }
                child_read_fd = open(child_fd_name, O_WRONLY);
                if(child_read_fd == -1){
                    ERROR_FUNC("open 2 error");
                }
                ret = add_driver(&head, &tail, child_read_fd, pid);
                if(ret == -1){
                    ERROR_FUNC("add driver error");
                }
                drivers_count++;
                continue;
            }
        }
        if(strncmp(input, "send_task", strcspn(input, " ")) == 0)
        {
            pid = atoi(strchr(input, ' '));
            if(pid <= 0){
                printf("error: wrong pid value\n");
                continue;
            }
            child_read_fd = get_fd(&head, pid);
            if(child_read_fd == -1){
                printf("error: pid doesn't exist\n");
                continue;
            }
            time = atoi(strrchr(input, ' '));
            if(time <= 0){
                printf("error: wrong time value\n");
                continue;
            }
            ret = send_task(child_read_fd, parent_read_fd, time);
            if(ret == -1){
                ERROR_FUNC("send task error");
            }
            continue;
        }
        if(strncmp(input, "get_status", strcspn(input, " ")) == 0)
        {
            pid = atoi(strchr(input, ' '));
            child_read_fd = get_fd(&head, pid);
            if(child_read_fd == -1){
                printf("error: pid doesn't exist\n");
                continue;
            }
            ret = get_status(child_read_fd, parent_read_fd);
            if(ret == -1){
                ERROR_FUNC("get status error");
            }
            continue;
        }
        if(strcmp(input, "get_drivers") == 0)
        {
            get_all_status(&head, parent_read_fd);
            if(ret == -1){
                ERROR_FUNC("get all status error");
            }
            continue;
        }
        if(strcmp(input, "exit") == 0)
        {
            break;
        }
        printf("error: unknown operation\n");
    }
    free_all(&head);
    close(parent_read_fd);
    close(child_read_fd);
    exit(EXIT_SUCCESS);
}