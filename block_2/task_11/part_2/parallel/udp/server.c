#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <err.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#define MSG_SIZE 10
#define N 20

int flag = 1;

struct thread_data{
    in_addr_t ip; // адрес клиента
    in_port_t port; // порт клиента
    int fd; // дескриптор потока
};


void *thread_job(void *arg){
    struct sockaddr_in cli_addr;
    int fd;
	char response[MSG_SIZE] = { 0 };
    time_t mytime = time(NULL);
    struct tm *now = localtime(&mytime);
    struct thread_data *data = (struct thread_data *) arg;
    int ret = 0;

    fd = data->fd;
    cli_addr.sin_addr.s_addr = data->ip;
    cli_addr.sin_port = data->port;
    cli_addr.sin_family = AF_INET;

    sprintf(response, "%d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);
    //sleep(1);
    ret = sendto(fd, response, MSG_SIZE, 0, (struct sockaddr *) &cli_addr, sizeof(cli_addr));
    if(ret < 0){
        perror("sendto thr error");
        pthread_exit(NULL);
    }
	close(fd);
	pthread_exit(NULL);
}

void sig_handler(int num, siginfo_t *info, void *args){
    printf("\nReceived signal #%d from %d\n", num, info->si_pid);
    flag = 0;
}

int main(){
	int ret = 0;
	struct sockaddr_in svr_addr, thr_serv_addr, cli_addr;
	socklen_t sin_len;
    pthread_attr_t thread_attr;
    pthread_t thread;
    int i;
    struct sigaction act;
    sigset_t set;
    char msg[MSG_SIZE];
    int sock;
    int cur_port, port;
    // Информация для потока
    // Массив нужен для того чтобы поток успел скопировать себе всю необходимую информацию
    // в случае одновременного подключения нескольких клиентов
    struct thread_data thr_data[N] = { 0 };

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

    ret = pthread_attr_init(&thread_attr);
    if(ret < 0){
        close(sock);
        err(EXIT_FAILURE, "attr init error");
    }
    ret = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if(ret < 0){
        close(sock);
        err(EXIT_FAILURE, "attr setdetachstate error");
    }

	ret = bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr));
	if (ret < 0){
        close(sock);
    	err(EXIT_FAILURE, "bind error");
    }

    i = 0;
	while (flag) {
        sin_len = sizeof(cli_addr);
        cur_port++;
    	ret = recvfrom(sock, msg, MSG_SIZE, 0, (struct sockaddr *) &cli_addr, &sin_len);
    	if (ret < 0) {
        	perror("recvfrom error");
        	continue;
    	}
    	else{
        	printf("got connection\n");
    	}

        thr_data[i].fd = socket(AF_INET, SOCK_DGRAM, 0);
        if(thr_data[i].fd < 0){
            perror("socket thread error");
            continue;
        }
        thr_serv_addr.sin_addr.s_addr = INADDR_ANY;
        thr_serv_addr.sin_family = AF_INET;
        thr_serv_addr.sin_port = htons(cur_port);

        while(1){
            ret = bind(thr_data[i].fd, (struct sockaddr *) &thr_serv_addr, sizeof(thr_serv_addr));
            if(ret < 0){
                if(errno == EADDRINUSE){
                    cur_port++;
                    thr_serv_addr.sin_port = htons(cur_port);
                    continue;
                }
                else{
                    perror("bind thread error");
                    close(thr_data[i].fd);
                    thr_serv_addr.sin_port = 0;
                    break;
                }
            }
            break;
        }
        if(thr_serv_addr.sin_port == 0){
            continue;
        }

        thr_data[i].ip = cli_addr.sin_addr.s_addr;
        thr_data[i].port = cli_addr.sin_port;

        memset(msg, 0, MSG_SIZE);
        sprintf(msg, "%d", cur_port);
        ret = sendto(sock, msg, MSG_SIZE, 0, (struct sockaddr *) &cli_addr, sin_len);
        if(ret < 0) {
        	perror("sendto error");
        	continue;
    	}

    	ret = pthread_create(&thread, &thread_attr, thread_job, (void*) &thr_data[i]);
    	if(ret < 0) {
        	perror("pthread create error");
        	continue;
    	}
        if(i == N - 1)
            i = 0;
        else
            i++;
	}
    close(sock);
    printf("End of work\n");
    exit(EXIT_SUCCESS);
}
