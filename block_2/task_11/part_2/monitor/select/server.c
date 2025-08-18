#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <err.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/select.h>

#define MSG_SIZE 10

int flag = 1;

void sig_handler(int num, siginfo_t *info, void *args){
    printf("\nReceived signal #%d from %d\n", num, info->si_pid);
    flag = 0;
}

int main() {
	int ret = 0;
	struct sockaddr_in svr_tcp_addr, svr_udp_addr, cli_addr;
	socklen_t sin_len = sizeof(cli_addr);
	int client_fd, TCP_sock, UDP_sock;
    struct sigaction act;
    sigset_t set;
    char msg[MSG_SIZE];
    time_t mytime;
    struct tm *now;
    fd_set rfds;
    int max_fd;

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sig_handler;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    act.sa_mask = set;
    ret = sigaction(SIGINT, &act, NULL);
    if (ret == -1) {
        err(EXIT_FAILURE, "sigaction error");
    }

	TCP_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (TCP_sock < 0){
    	err(EXIT_FAILURE, "socket 1 error");
    }
    UDP_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (UDP_sock < 0){
        close(TCP_sock);
    	err(EXIT_FAILURE, "socket 2 error");
    }

	//const int enable = 1;
	//setsockopt(TCP_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    //setsockopt(UDP_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	svr_tcp_addr.sin_family = svr_udp_addr.sin_family= AF_INET;
	svr_tcp_addr.sin_addr.s_addr =  svr_udp_addr.sin_addr.s_addr = INADDR_ANY;
	svr_tcp_addr.sin_port = htons(5050);
    svr_udp_addr.sin_port = htons(5051);

	ret = bind(TCP_sock, (struct sockaddr *) &svr_tcp_addr, sizeof(svr_tcp_addr));
    if(ret < 0){
    	close(TCP_sock);
        close(UDP_sock);
    	err(EXIT_FAILURE, "bind 1 error");
	}
    ret = bind(UDP_sock, (struct sockaddr *) &svr_udp_addr, sizeof(svr_udp_addr));
    if(ret < 0){
    	close(TCP_sock);
        close(UDP_sock);
    	err(EXIT_FAILURE, "bind 2 error");
	}

	ret = listen(TCP_sock, 20);
    if(ret < 0){
    	close(TCP_sock);
        close(UDP_sock);
    	err(EXIT_FAILURE, "listen error");
	}

    max_fd = TCP_sock > UDP_sock ? TCP_sock : UDP_sock;

    while(flag){
        FD_ZERO(&rfds);
        FD_SET(TCP_sock, &rfds);
        FD_SET(UDP_sock, &rfds);
        ret = select(max_fd + 1, &rfds, NULL, NULL, NULL);
        if(ret < 0){
            close(TCP_sock);
            close(UDP_sock);
            err(EXIT_FAILURE, "epoll wait error");
        }
        if(FD_ISSET(TCP_sock, &rfds)){
            client_fd = accept(TCP_sock, (struct sockaddr *)  &cli_addr, &sin_len);
            if(client_fd < 0){
                perror("accept error");
                continue;
            }
            printf("got connection\n");
            memset(msg, 0, MSG_SIZE);
            mytime = time(NULL);
            now = localtime(&mytime);
            sprintf(msg, "%d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);
            ret = send(client_fd, msg, MSG_SIZE, 0);
            if(ret < 0){
                perror("send error");
                close(client_fd);
                continue;
            }
            close(client_fd);
        }
        if(FD_ISSET(UDP_sock, &rfds)){
            ret = recvfrom(UDP_sock, msg, MSG_SIZE, 0, (struct sockaddr *) &cli_addr, &sin_len);
            if(ret < 0){
                perror("recvfrom error");
                continue;
            }
            printf("got connection\n");
            memset(msg, 0, MSG_SIZE);
            mytime = time(NULL);
            now = localtime(&mytime);
            sprintf(msg, "%d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);
            ret = sendto(UDP_sock, msg, MSG_SIZE, 0, (struct sockaddr *) &cli_addr, sin_len);
            if(ret < 0){
                perror("sendto error");
                continue;
            }
        }
    }
    close(TCP_sock);
    close(UDP_sock);
    printf("end of work\n");
    exit(EXIT_SUCCESS);
}
