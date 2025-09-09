#ifndef __CLIENT_FUNC_H__
#define __CLIENT_FUNC_H__

#include <stdint.h>

#define CLIENT_MSG_LEN 20
#define SERVER_MSG_LEN 30
#define HEADERS_SIZE 28

struct frame{
    uint16_t source_port;
    uint16_t dest_port;
    uint16_t len;
    uint16_t checksum;
    char payload[CLIENT_MSG_LEN];
};

// Функция ввода строки
int input_string(char *str, int size);

#endif