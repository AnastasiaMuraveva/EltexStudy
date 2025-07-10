CC=gcc

CFLAGS=-c -Wall -Wextra

all: main

main: main.o operations.o
	$(CC) main.o operations.o -o main

operations.o: operations.c
	$(CC) $(CFLAGS) operations.c

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o
