CC=gcc
CFLAGS=-Wall -Wextra -g

all: main sender

main: main.o 
	$(CC) main.o -o main
	
sender: sender.o 
	$(CC) sender.o -o sender
	
main.o: main.c 
	$(CC) $(CFLAGS) -c main.c
	
sender.o: sender.c
	$(CC) $(CFLAGS) -c sender.c

clean:
	rm -rf *.o main sender
