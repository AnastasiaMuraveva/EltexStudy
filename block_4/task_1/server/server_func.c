#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#include "server_func.h"

int get_client_count(struct client_info **clients, in_addr_t ip, in_port_t port){
    int last_byte = (ntohl(ip) & 0xff) - 1;
    struct client_info *tmp;

    if(clients[last_byte] == NULL){
        tmp = calloc(1, sizeof(struct client_info));
        tmp->ip = ip;
        tmp->count = 0;
        tmp->port = port;
        tmp->next = NULL;
        clients[last_byte] = tmp;
        return tmp->count;
    }

    tmp = clients[last_byte];
    while(1){
        if(tmp->ip != ip || tmp->port != port){
            if(tmp->next == NULL){
                tmp->next = calloc(1, sizeof(struct client_info));
                tmp->next->ip = ip;
                tmp->next->count = 0;
                tmp->next->port = port;
                tmp->next->next = NULL;
                return tmp->next->count;
            }
            tmp = tmp->next;
        }
        else{
            tmp->count++;
            return tmp->count;
        }
    }
}

void delete_client(struct client_info **clients, in_addr_t ip, in_port_t port){
    int last_byte = (ntohl(ip) & 0xff) - 1;
    struct client_info *tmp = clients[last_byte];
    struct client_info *tmp2 = tmp->next;

    if(tmp == NULL){
        printf("Client was not found\n");
        return;
    }

    // Если удаляем единственный элемент
    if(tmp->next == NULL && tmp->ip == ip && tmp->port == port){
        free(tmp);
        clients[last_byte] = NULL;
        printf("Client is deleted\n");
        return;
    }

    // Если удаляем первый элемент
    if(tmp->next != NULL && tmp->ip == ip && tmp->port == port){
        clients[last_byte] = tmp->next;
        free(tmp);
        printf("Client is deleted\n");
        return;
    }

    while(tmp2 != NULL){
        if(tmp2->ip == ip && tmp2->port == port){
            tmp->next = tmp2->next;
            free(tmp2);
            printf("Client is deleted\n");
            return;
        }
        tmp = tmp->next;
        tmp2 = tmp2->next;
    }
    printf("Client was not found\n");
    return;
}

void send_exit_message(struct client_info **clients, in_port_t serv_port, int fd){
    struct client_info *tmp;
    int ret = 0;

    struct frame udp_frame;
    struct sockaddr_in client;

    client.sin_family = AF_INET;

    udp_frame.source_port = htons(serv_port);
    udp_frame.len = htons(sizeof(udp_frame));
    udp_frame.checksum = 0;

    strcpy(udp_frame.payload, "exit");

    for(int i = 0; i < ARRAY_SIZE; i++){
        tmp = clients[i];
        while(tmp != NULL){
            udp_frame.dest_port = tmp->port;
            client.sin_addr.s_addr = tmp->ip;
            client.sin_port = tmp->port;
            ret = sendto(fd, &udp_frame, sizeof(udp_frame), 0, (struct sockaddr *) &client, sizeof(client));
            if(ret == -1){
                perror("send exit message sendto error");
            }
            tmp = tmp->next;
        }
    }
}

void free_all(struct client_info **clients){
    struct client_info *tmp;
    for(int i = 0; i < ARRAY_SIZE; i++){
        tmp = clients[i];
        while(tmp != NULL){
            clients[i] = clients[i]->next;
            free(tmp);
            tmp = clients[i];
        }
    }
}