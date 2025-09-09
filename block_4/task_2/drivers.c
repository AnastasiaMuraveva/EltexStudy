#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "drivers.h"

#define ERROR_FUNC(str){perror(str); \
                        return -1;}

int driver_job(int child_fd, int parent_fd){
    int ret = 0;
    time_t time;
    char buf[MSG_SIZE] = { 0 };
    struct epoll_event ev, events[MAX_EVENTS];
    int nfds, epollfd;
    int tfd;
    int is_exit = 0;
    int is_work_over = 0;
    struct itimerspec its = {
        .it_value = {.tv_sec = 0, .tv_nsec = 0}, 
        .it_interval = {.tv_sec = 0, .tv_nsec = 0}   // без повторов
    }; 

    tfd = timerfd_create(CLOCK_REALTIME, 0);
    if (tfd == -1) {
        ERROR_FUNC("driver job timerfd create error");
    }

    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        ERROR_FUNC("driver job epoll create1 error");
    }
    ev.events = EPOLLIN;
    ev.data.fd = tfd;
    ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, tfd, &ev);
    if (ret == -1) {
        ERROR_FUNC("driver job epoll ctl 1 error");
    }

    ev.data.fd = child_fd;
    ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, child_fd, &ev);
    if (ret == -1) {
        ERROR_FUNC("driver job epoll ctl 2 error");
    }

    while(!is_exit){
        ret = read(child_fd, buf, MSG_SIZE);
        if (ret == -1) {
            ERROR_FUNC("driver job read 1 error");
        }
        if(strcmp(buf, "status") == 0){
            strcpy(buf, "available");
            ret = write(parent_fd, buf, MSG_SIZE);
            if (ret == -1) {
                ERROR_FUNC("driver job write 1 error");
            }
            continue;
        }
        else if(strcmp(buf, "exit") == 0){
            return 0;
        }
        else{
            time = atoi(buf);
            if(time <= 0){
                printf("driver job error: wrong time value\n");
                continue;
            }
            its.it_value.tv_sec = time;
            strcpy(buf, "ok");
            ret = write(parent_fd, buf, MSG_SIZE);
            if (ret == -1) {
                ERROR_FUNC("driver job write 2 error");
            }
            is_work_over = 0;
            ret = timerfd_settime(tfd, 0, &its, NULL);
            if (ret == -1) {
                ERROR_FUNC("driver job timerfd settime error");
            }
        }
        // Во время работы перехватываем сообщения родительского процесса
        // и отправляем оставшееся до окончания работы время
        // При получении сообщения о завершении, процесс сначала заканчивает текущую работу
        while(!is_work_over){
            nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
            if (nfds == -1) {
                ERROR_FUNC("driver job epoll wait error");
            }
            for(int i = 0; i < nfds; i++){
                if(events->data.fd == child_fd){
                    ret = read(child_fd, buf, MSG_SIZE);
                    if (ret == -1) {
                        ERROR_FUNC("driver job read 2 error");
                    }
                    if(strcmp(buf, "exit") == 0){
                        is_exit = 1;
                    }
                    else{
                        ret = timerfd_gettime(tfd, &its);
                        if (ret == -1) {
                            ERROR_FUNC("driver timerfd gettime error");
                        }   
                        time = its.it_value.tv_sec;
                        sprintf(buf, "busy %ld", time);
                        ret = write(parent_fd, buf, MSG_SIZE);
                        if (ret == -1) {
                            ERROR_FUNC("driver job write 3 error");
                        }       
                    }
                }
                else{
                    is_work_over = 1;
                }
            }
        }
    }
    return 0;
}