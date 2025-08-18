#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <err.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#define MSG_SIZE 10
#define THREADS_NUM 10

int flag = 1;

void sig_handler(int num, siginfo_t *info, void *args){
    printf("\nReceived signal #%d from %d\n", num, info->si_pid);
    flag = 0;
}

void *thread_job(void *arg){
    int *is_free = (int *) arg;
    char response[MSG_SIZE] = { 0 };
    int ret = 0;
    time_t mytime;
    struct tm *now;

    while(1){
        if(*is_free == 0){
            usleep(500);
            continue;
        }
        if(*is_free == -1){
            printf("end of thread's work\n");
            pthread_exit(NULL);
        }
        memset(response, 0, MSG_SIZE);
        mytime = time(NULL);
        now = localtime(&mytime);
        sprintf(response, "%d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);
        //sleep(1);
        ret = send(*is_free, response, MSG_SIZE, 0);
        if(ret < 0){
            perror("send error");
            pthread_exit(NULL);
        }
        close(*is_free);
        *is_free = 0;
    }
	pthread_exit(NULL);
}

int main() {
	int ret = 0;
    pthread_t threads[THREADS_NUM];
	struct sockaddr_in svr_addr, cli_addr;
	socklen_t sin_len = sizeof(cli_addr);
	int client_fd;
    struct sigaction act;
    sigset_t set;
    int is_thread_free[THREADS_NUM] = { 0 }; // если поток занят, элемент массива будет содержать дескриптор для клиента

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sig_handler;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    act.sa_mask = set;
    ret = sigaction(SIGINT, &act, NULL);
    if (ret == -1) {
        err(EXIT_FAILURE, "sigaction error");
    }

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0){
    	err(EXIT_FAILURE, "socket error");
    }

    //const int enable = 1;
	//setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	int port = 5050;
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = INADDR_ANY;
	svr_addr.sin_port = htons(port);

	ret = bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr));
    if(ret < 0){
    	close(sock);
    	err(EXIT_FAILURE, "bind error");
	}

	ret = listen(sock, 20);
    if(ret < 0){
    	close(sock);
    	err(EXIT_FAILURE, "listen error");
	}

    for (int i = 0; i < THREADS_NUM; i++) {
    	ret = pthread_create(&threads[i], NULL, thread_job, &is_thread_free[i]);
    	if (ret < 0) {
            for(int j = 0; j < i; j++){
                is_thread_free[i] = -1;
                pthread_join(threads[j], NULL);
            }
        	close(sock);
            err(EXIT_FAILURE, "pthread create error");
        }   
    }

    while(flag){
        client_fd = accept(sock, (struct sockaddr *)  &cli_addr, &sin_len);
        if(client_fd < 0){
            perror("accept error");
            continue;
        }
        printf("got connection\n");
        for(int i = 0; i < THREADS_NUM; i++){
            if (is_thread_free[i] != 0)
                continue;
            else{
                is_thread_free[i] = client_fd;
                break;
            }
            if(i == THREADS_NUM - 1){
                printf("All threads are busy. Request will be dropped\n");
                close(client_fd);
            }
        }
    }
    close(sock);
    for(int i = 0; i < THREADS_NUM; i++){
        while(1){
            if(is_thread_free[i] != 0){
                usleep(200);
                continue;
            } 
            is_thread_free[i] = -1;
            break;
        }
    }
    for(int i = 0; i < THREADS_NUM; i++){
        pthread_join(threads[i], NULL);
    }
    printf("end of work\n");
    exit(EXIT_SUCCESS);
}
