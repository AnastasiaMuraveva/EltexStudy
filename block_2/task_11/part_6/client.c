#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <arpa/inet.h>

#define STR_LEN 8

struct udpframe{
    uint16_t source_port;
    uint16_t dest_port;
    uint16_t len;
    uint16_t checksum;
    char payload[STR_LEN];
};

struct ipframe{
    uint8_t ver_head_len;
    uint8_t d_serv;
    uint16_t total_len;
    uint16_t id;
    uint16_t flags_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t ip_source;
    uint32_t ip_dest;
    struct udpframe udp_frame;
};

int main(){
    struct sockaddr_in serv;
    socklen_t sin_len = sizeof(serv);
    int fd;
    struct ipframe ip_frame;
    int msg_len = sizeof(ip_frame);
    char msg[msg_len];
    unsigned short *data;
    int ret = 0;
    int enable = 1;

    // Версия протокола = 4
    uint8_t ver = 4;
    ver = ver << 4;
    // Наш IP-заголовок содержит 5 32-битных слов
    ip_frame.ver_head_len = ver | 5;
    // DS устанавливаем в 0, т.к. приоритет нам не важен
    ip_frame.d_serv = 0;
    // Общая длина будет заполнена автоматически
    ip_frame.total_len = 0;
    // Идентификатор будет заполнен автоматически
    ip_frame.id = 0;
    // Флаги и смещение равны 0, т.к. мы отправляем единственный пакет
    ip_frame.flags_offset = 0;
    // 64 - стандартное значение времени жизни UDP-пакета
    ip_frame.ttl = 64;
    // Номер UDP-протокола согласно IANA = 17
    ip_frame.protocol = 17;
    // Контрольная сумма будет заполнена автоматически
    ip_frame.checksum = 0;
    // IP отправителя будет заполнен автоматически
    ip_frame.ip_source = 0;
    ip_frame.ip_dest = inet_addr("127.0.0.1");


    ip_frame.udp_frame.source_port = htons(7777);
    ip_frame.udp_frame.dest_port = htons(5050);
    ip_frame.udp_frame.len = htons(sizeof(ip_frame.udp_frame));
    ip_frame.udp_frame.checksum = 0;
    strcpy(ip_frame.udp_frame.payload, "Hi");

    fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(fd == -1){
        err(EXIT_FAILURE, "socket error");
    }
    ret = setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable));
    if(ret == -1){
        close(fd);
        err(EXIT_FAILURE, "setsockopt error");
    }

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = ip_frame.ip_dest;
    serv.sin_port = ip_frame.udp_frame.dest_port;

    ret = sendto(fd, &ip_frame, sizeof(ip_frame), 0, (struct sockaddr *) &serv, sizeof(serv));
    if(ret == -1){
        close(fd);
        err(EXIT_FAILURE, "sendto error");
    }

    while(1){
        memset(msg, 0, msg_len);
        ret = recvfrom(fd, msg, msg_len, 0, (struct sockaddr *) &serv, &sin_len);
        if(ret == -1){
            close(fd);
            err(EXIT_FAILURE, "recvfrom error");
        }
        data = msg;
        data += 11;
        if(ntohs(*data) == 7777)
            break;
    }

    printf("Client receive: %s\n", &msg[msg_len - STR_LEN]);

    close(fd);
    exit(EXIT_SUCCESS);
}
