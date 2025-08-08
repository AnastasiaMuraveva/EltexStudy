CC=gcc
CFLAGS=-Wall -Wextra -g
LDFLAGS := -lrt -pthread

all: server client

server: server.o 
	$(CC) server.o -o server $(LDFLAGS)
	
client: client.o 
	$(CC) client.o -o client $(LDFLAGS)
	
server.o: server.c 
	$(CC) $(CFLAGS) -c server.c
	
client.o: client.c
	$(CC) $(CFLAGS) -c client.c

clean:
	rm -rf *.o server client
