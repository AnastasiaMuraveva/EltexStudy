#ifndef __SERVER_FUNC_H__
#define __SERVER_FUNC_H__

#include <stdint.h>
#include <netinet/in.h>

#define CLIENT_MSG_LEN 20
#define SERVER_MSG_LEN 30
#define HEADERS_SIZE 28
#define ARRAY_SIZE 254

struct frame{
    uint16_t source_port;
    uint16_t dest_port;
    uint16_t len;
    uint16_t checksum;
    char payload[SERVER_MSG_LEN];
};

struct client_info{
    in_addr_t ip;
    in_port_t port;
    int count;
    struct client_info *next;
};

// Функция получения порядкового номера сообщения клиента, если клиента еще нет в списке, добавляет его
int get_client_count(struct client_info **clients, in_addr_t ip, in_port_t port);

// Функция удаления клиента из списка
void delete_client(struct client_info **clients, in_addr_t ip, in_port_t port);

// Функция, отправляющая клиентам сообщение о завершении работы сервера
void send_exit_message(struct client_info **clients, in_port_t serv_port, int fd);

// Функция освобождения всей выделенной памяти
void free_all(struct client_info **clients);

#endif