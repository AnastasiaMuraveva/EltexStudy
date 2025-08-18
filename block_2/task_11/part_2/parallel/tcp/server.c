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
#define MSG_SIZE 10
#define N 20

int flag = 1;

void *thread_job(void *arg){
	int fd = *(int*) arg;
	char response[MSG_SIZE] = { 0 };
    time_t mytime = time(NULL);
    struct tm *now = localtime(&mytime);
    sprintf(response, "%d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);
    //sleep(1);
    send(fd, response, MSG_SIZE, 0);
	close(fd);
	pthread_exit(NULL);
}

void sig_handler(int num, siginfo_t *info, void *args){
    printf("\nReceived signal #%d from %d\n", num, info->si_pid);
    flag = 0;
}

int main(){
	int ret = 0;
	struct sockaddr_in svr_addr, cli_addr;
	socklen_t sin_len = sizeof(cli_addr);
	int client_fd[N];
    pthread_attr_t thread_attr;
    pthread_t thread;
    int i;
    struct sigaction act;
    sigset_t set;

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

	ret = listen(sock, N);
    if (ret < 0){
        close(sock);
    	err(EXIT_FAILURE, "listen error");
    }

    i = 0;
	while (flag) {
    	client_fd[i] = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);
    	if (client_fd[i] < 0) {
        	perror("accept error");
        	continue;
    	}
    	else{
        	printf("got connection\n");
    	}
    	ret = pthread_create(&thread, &thread_attr, thread_job, (void*) &client_fd[i]);
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
