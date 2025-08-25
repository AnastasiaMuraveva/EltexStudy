#include <stdio.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/ethernet.h>

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

struct etherframe{
    uint16_t unused; // используется для контроля выравнивания, игнорируется при отправке
    uint8_t mac_dest[6];
    uint8_t mac_source[6];
    uint16_t type;
    struct ipframe ip_frame;
};

int main(){
    struct sockaddr_ll serv = { 0 };
    socklen_t sin_len = sizeof(serv);
    int fd;
    struct etherframe ether_frame;
    int msg_len = sizeof(ether_frame) - 2;
    char msg[msg_len];
    uint16_t *ptr, tmp;
    char *ether_ptr;
    int ret = 0;
    int sum = 0;
    uint8_t mac_source[] = {0x24, 0x2f, 0xd0, 0x8a, 0x8c, 0x95};
    uint8_t mac_dest[] = {0xf8, 0x28, 0x19, 0x6b, 0xd4, 0xa3};

    // ethernet заголовок
    ether_frame.type = htons(0x0800);
    for(int i = 0; i < 6; i++){
        ether_frame.mac_source[i] = mac_source[i];
        ether_frame.mac_dest[i] = mac_dest[i];
    }

    // ip заголовок
    uint8_t ver = 4;
    ver = ver << 4;
    ether_frame.ip_frame.ver_head_len = ver | 5;
    ether_frame.ip_frame.d_serv = 0;
    ether_frame.ip_frame.total_len = htons(sizeof(struct ipframe));
    ether_frame.ip_frame.id = 12345;
    ether_frame.ip_frame.flags_offset = 0;
    ether_frame.ip_frame.ttl = 64;
    ether_frame.ip_frame.protocol = 17;
    ether_frame.ip_frame.checksum = 0;
    ether_frame.ip_frame.ip_source = inet_addr("192.168.1.113");
    ether_frame.ip_frame.ip_dest = inet_addr("192.168.1.102");

    ptr = (unsigned short *) &ether_frame.ip_frame;
    for(int i = 0; i < 10; i++){
        sum += *ptr;
        ptr++;
    }
    tmp = sum >> 16;
    sum = (sum & 0xFFFF) + tmp;
    sum = ~sum;
    ether_frame.ip_frame.checksum = sum;

    // udp заголовок
    ether_frame.ip_frame.udp_frame.source_port = htons(7777);
    ether_frame.ip_frame.udp_frame.dest_port = htons(5050);
    ether_frame.ip_frame.udp_frame.len = htons(sizeof(struct udpframe));
    ether_frame.ip_frame.udp_frame.checksum = 0;
    strcpy(ether_frame.ip_frame.udp_frame.payload, "Hi");

    fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(fd == -1){
        err(EXIT_FAILURE, "socket error");
    }

    serv.sll_family = AF_PACKET;
    serv.sll_ifindex = if_nametoindex("wlx242fd08a8c95");
    serv.sll_halen = 6;
    for(int i = 0; i < 6; i++){
        serv.sll_addr[i] = mac_dest[i];
    }

    // Игнорируем переменную unused
    ether_ptr = (char *)&ether_frame.mac_dest;
    ret = sendto(fd, ether_ptr, msg_len, 0, (struct sockaddr *) &serv, sin_len);
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
        ptr = (unsigned short *) msg;
        ptr += 18;
        if(ntohs(*ptr) == 7777)
            break;
    }

    printf("Client receive: %s\n", &msg[msg_len - STR_LEN]);

    close(fd);
    exit(EXIT_SUCCESS);
}
