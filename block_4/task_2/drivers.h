#ifndef __DRIVERS_H__
#define __DRIVERS_H__

#define MSG_SIZE 20
#define MAX_EVENTS 2

struct driver_info{
    int pid;
    int fd;
};

struct drivers_list{
    struct drivers_list *next;
    struct driver_info data;
};

int driver_job(int child_fd, int parent_fd);

#endif