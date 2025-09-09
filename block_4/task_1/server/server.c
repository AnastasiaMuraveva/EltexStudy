#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <signal.h>
#include <errno.h>

#include "server_func.h"

// Используем сигнал для прерывания работы сервера 
// и последующего корректного освобождения памяти
int flag = 1;
void sig_handler(int num, siginfo_t *info, void *args){
    printf("\nReceived signal #%d from %d\n", num, info->si_pid);
    flag = 0;
}

int main(){
    struct sockaddr_in client;
    socklen_t sin_len = sizeof(client);
    int client_fd;
    struct frame udp_frame = { 0 };
    int msg_len = CLIENT_MSG_LEN + HEADERS_SIZE;
    char msg[msg_len];
    unsigned short *ptr;
    // В качестве структуры для хранения информации о клиентах используем массив списков
    // Элемент массива, в который будет помещаться информация о клиенте 
    // выбирается в соответствии с последним байтом ip-адреса
    // Если у нескольких клиентов этот байт совпадает формируется список
    struct client_info *clients[ARRAY_SIZE] = { 0 };
    int count;
    int ret = 0;
    in_port_t client_port;
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

    // Заполняем заголовок, порт получателя заполним позже
    udp_frame.source_port = htons(5050);
    udp_frame.len = htons(sizeof(udp_frame));
    udp_frame.checksum = 0;

    client_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(client_fd == -1){
        err(EXIT_FAILURE, "socket error");
    }

    client.sin_family = AF_INET;

    while(1){
        while(flag){
            ret = recvfrom(client_fd, msg, msg_len, 0, (struct sockaddr *) &client, &sin_len);
            if(ret == -1 && errno != EINTR){
                close(client_fd);
                perror("recvfrom error");
                continue;
            }
            ptr = (unsigned short *) msg;
            ptr += 11;
            if(ntohs(*ptr) == 5050)
                break;
        }
        if(!flag){
            break;
        }
        ptr = (unsigned short *) msg;
        ptr += 10;
        client_port = *ptr;
        if(strcmp(&msg[HEADERS_SIZE], "exit") == 0){
            delete_client(clients, client.sin_addr.s_addr, client_port);
            continue;
        }
        printf("Server receive: %s\n", &msg[HEADERS_SIZE]);
        // Первое сообщение от пользователя - номер его порта
        // на это сообщение не отвечаем
        count = get_client_count(clients, client.sin_addr.s_addr, client_port);
        if(count == 0){
            printf("New client is added\n");
            continue;
        }
        sprintf(udp_frame.payload, "%s %d", &msg[HEADERS_SIZE], count);
        udp_frame.dest_port = client_port;
        client.sin_port = client_port;
        ret = sendto(client_fd, &udp_frame, sizeof(udp_frame), 0, (struct sockaddr *) &client, sin_len);
        if(ret == -1){
            close(client_fd);
            perror("sendto error");
            continue;
        }
    }
    send_exit_message(clients, 5050, client_fd);
    free_all(clients);
    close(client_fd);
    exit(EXIT_SUCCESS);
}
