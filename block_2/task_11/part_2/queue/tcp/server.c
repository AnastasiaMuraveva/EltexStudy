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
#include <mqueue.h>

#define MSG_SIZE 10
#define Q_NAME "/qserv"
#define THREADS_NUM 10

int flag = 1;

void sig_handler(int num, siginfo_t *info, void *args){
    printf("\nReceived signal #%d from %d\n", num, info->si_pid);
    flag = 0;
}

void *thread_job(void *arg){
    char q_msg[MSG_SIZE] = { 0 };
    int fd;
    int ret = 0;
    time_t mytime;
    struct tm *now;
    mqd_t qid;

    qid = mq_open(Q_NAME, O_RDONLY);
    while(1){
        ret = mq_receive(qid, q_msg, MSG_SIZE, NULL);
        if(ret < 0){
            perror("mq receive error");
            pthread_exit(NULL);
        }
        fd = atoi(q_msg);
        if(fd == 0){
            if(strcmp(q_msg, "exit") == 0){
                printf("end of thread's work\n");
                pthread_exit(NULL);
            }
            else{
                perror("atoi error");
                pthread_exit(NULL);
            }
        }
        memset(q_msg, 0, MSG_SIZE);
        mytime = time(NULL);
        now = localtime(&mytime);
        sprintf(q_msg, "%d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);
        //sleep(1);
        ret = send(fd, q_msg, MSG_SIZE, 0);
        if(ret < 0){
            perror("send error");
            pthread_exit(NULL);
        }
        close(fd);
    }
	pthread_exit(NULL);
}

int main() {
	int ret = 0;
    pthread_t threads[THREADS_NUM];
	struct sockaddr_in svr_addr, cli_addr;
	socklen_t sin_len = sizeof(cli_addr);
	int client_fd;
    char q_msg[MSG_SIZE];
    struct sigaction act;
    sigset_t set;
    mqd_t qid;
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_SIZE;
    attr.mq_curmsgs = 0;

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sig_handler;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    act.sa_mask = set;
    ret = sigaction(SIGINT, &act, NULL);
    if (ret == -1) {
        err(EXIT_FAILURE, "sigaction error");
    }

    mq_unlink(Q_NAME);
    qid = mq_open(Q_NAME, O_WRONLY | O_CREAT | O_EXCL, 0644, &attr);
    if(qid < 0){
        perror("mq open error");
        exit(EXIT_FAILURE);
    }

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0){
        mq_close(qid);
        mq_unlink(Q_NAME);
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
        mq_close(qid);
        mq_unlink(Q_NAME);
    	err(EXIT_FAILURE, "bind error");
	}

	ret = listen(sock, 20);
    if(ret < 0){
    	close(sock);
        mq_close(qid);
        mq_unlink(Q_NAME);
    	err(EXIT_FAILURE, "listen error");
	}

    for (int i = 0; i < THREADS_NUM; i++) {
    	ret = pthread_create(&threads[i], NULL, thread_job, NULL);
    	if (ret < 0) {
            strcpy(q_msg, "exit");
            for(int j = 0; j < i; j++){
                ret = mq_send(qid, q_msg, MSG_SIZE, 1);
            }
            for(int j = 0; j < i; j++){
                pthread_join(threads[j], NULL);
            }
        	close(sock);
            mq_close(qid);
            mq_unlink(Q_NAME);
            err(EXIT_FAILURE, "setsockopt error");
        }   
    }

    while(flag){
        client_fd = accept(sock, (struct sockaddr *)  &cli_addr, &sin_len);
        if(client_fd < 0){
            perror("accept error");
            continue;
        }
        printf("got connection\n");
        sprintf(q_msg, "%d", client_fd);
        ret = mq_send(qid, q_msg, MSG_SIZE, 1);
        if(ret < 0){
            perror("mq send error");
            continue;
        }
    }
    close(sock);
    strcpy(q_msg, "exit");
    for(int i = 0; i < THREADS_NUM; i++){
        ret = mq_send(qid, q_msg, MSG_SIZE, 1);
    }
    for(int i = 0; i < THREADS_NUM; i++){
        pthread_join(threads[i], NULL);
    }
    mq_close(qid);
    mq_unlink(Q_NAME);
    printf("end of work\n");
    exit(EXIT_SUCCESS);
}
