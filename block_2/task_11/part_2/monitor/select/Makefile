CC=gcc
CFLAGS=-Wall -Wextra -g

all: server tcp_client udp_client

server: server.o 
	$(CC) server.o -o server $(LDFLAGS)
	
tcp_client: tcp_client.o 
	$(CC) tcp_client.o -o tcp_client $(LDFLAGS)
	
udp_client: udp_client.o 
	$(CC) udp_client.o -o udp_client $(LDFLAGS)
	
server.o: server.c 
	$(CC) $(CFLAGS) -c server.c
	
tcp_client.o: tcp_client.c
	$(CC) $(CFLAGS) -c tcp_client.c
	
udp_client.o: udp_client.c
	$(CC) $(CFLAGS) -c udp_client.c

clean:
	rm -rf *.o server tcp_client udp_client
