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
#include <errno.h>

#define MSG_SIZE 10
#define THREADS_NUM 10

int flag = 1;

struct thread_data{
    in_addr_t ip; // адрес клиента
    in_port_t port; // порт клиента
    int fd; // дескриптор потока
    int serv_port; // порт потока
    int is_free; // переменная для проверки, занят ли поток
};

void sig_handler(int num, siginfo_t *info, void *args){
    printf("\nReceived signal #%d from %d\n", num, info->si_pid);
    flag = 0;
}

void *thread_job(void *arg){
    struct thread_data *data = (struct thread_data *) arg;
    struct sockaddr_in cli_addr;
    char response[MSG_SIZE] = { 0 };
    int ret = 0;
    time_t mytime;
    struct tm *now;

    while(1){
        if(data->is_free == 0){
            usleep(500);
            continue;
        }
        if(data->is_free == -1){
            printf("end of thread's work\n");
            close(data->fd);
            pthread_exit(NULL);
        }

        cli_addr.sin_addr.s_addr = data->ip;
        cli_addr.sin_port = data->port;
        cli_addr.sin_family = AF_INET;

        memset(response, 0, MSG_SIZE);
        mytime = time(NULL);
        now = localtime(&mytime);
        sprintf(response, "%d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);
        //sleep(1);
        ret = sendto(data->fd, response, MSG_SIZE, 0, (struct sockaddr *) &cli_addr, sizeof(cli_addr));
        if(ret < 0){
            perror("sendto thread error");
            close(data->fd);
            pthread_exit(NULL);
        }
        data->is_free = 0;
    }
}

int main() {
	int ret = 0;
    pthread_t threads[THREADS_NUM];
	struct sockaddr_in svr_addr, cli_addr, thr_srv_addr;
	socklen_t sin_len = sizeof(cli_addr);
    struct sigaction act;
    sigset_t set;
    char msg[MSG_SIZE];
    struct thread_data thr_data[THREADS_NUM] = { 0 };
    int port, cur_port;
    int sock;

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sig_handler;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    act.sa_mask = set;
    ret = sigaction(SIGINT, &act, NULL);
    if (ret == -1) {
        err(EXIT_FAILURE, "sigaction error");
    }

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0){
    	err(EXIT_FAILURE, "socket error");
    }

    //const int enable = 1;
	//setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	port = cur_port = 5050;
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = INADDR_ANY;
	svr_addr.sin_port = htons(port);

	ret = bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr));
    if(ret < 0){
    	close(sock);
    	err(EXIT_FAILURE, "bind error");
	}

    for (int i = 0; i < THREADS_NUM; i++) {
        cur_port++;
        thr_data[i].fd = socket(AF_INET, SOCK_DGRAM, 0);
        thr_srv_addr.sin_addr.s_addr = INADDR_ANY;
        thr_srv_addr.sin_port = htons(cur_port);
        thr_srv_addr.sin_family = AF_INET;
        while(1){
            ret = bind(thr_data[i].fd, (struct sockaddr *) &thr_srv_addr, sizeof(thr_srv_addr));
            if(ret < 0){
                if(errno == EADDRINUSE){
                    cur_port++;
                    continue;
                }
                else{
                    close(sock);
                    for(int j = 0; j < i; j++){
                        thr_data[j].is_free = -1;
                        pthread_join(threads[j], NULL);
                    }
                    err(EXIT_FAILURE, "bind thread error");
                }
            }
            thr_data[i].serv_port = cur_port;
            break;
        }
    	ret = pthread_create(&threads[i], NULL, thread_job, &thr_data[i]);
    	if (ret < 0) {
        	close(sock);
            for(int j = 0; j < i; j++){
                thr_data[j].is_free = -1;
                pthread_join(threads[j], NULL);
            }
            err(EXIT_FAILURE, "pthread create error");
        }   
    }

    while(flag){
        memset(msg, 0, MSG_SIZE);
        ret = recvfrom(sock, msg, MSG_SIZE, 0, (struct sockaddr *) &cli_addr, &sin_len);
        if(ret < 0){
            perror("recvfrom error");
            continue;
        }
        printf("got connection\n");
        for(int i = 0; i < THREADS_NUM; i++){
            if (thr_data[i].is_free != 0)
                continue;
            else{
                thr_data[i].ip = cli_addr.sin_addr.s_addr;
                thr_data[i].port = cli_addr.sin_port;
                sprintf(msg, "%d", thr_data[i].serv_port);
                ret = sendto(sock, msg, MSG_SIZE, 0, (struct sockaddr *) &cli_addr, sizeof(cli_addr));
                if(ret < 0){
                    perror("sendto error");
                    continue;
                }
                thr_data[i].is_free = 1;
                break;
            }
            if(i == THREADS_NUM - 1){
                printf("All threads are busy. Request will be dropped\n");
            }
        }
    }
    close(sock);
    for(int i = 0; i < THREADS_NUM; i++){
        while(1){
            if(thr_data[i].is_free != 0){
                usleep(200);
                continue;
            } 
            thr_data[i].is_free = -1;
            break;
        }
    }
    for(int i = 0; i < THREADS_NUM; i++){
        pthread_join(threads[i], NULL);
    }
    printf("end of work\n");
    exit(EXIT_SUCCESS);
}
