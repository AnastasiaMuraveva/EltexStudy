BIN := client
OBJ := client.o client_func.o
SRC := client.c client_func.c

CC := gcc
CFLAGS := -g -Wall -Wextra
#LDFLAGS := 

.PHONY : clean

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)
	
%.o: %.c
	$(CC) $^ $(CFLAGS) $^ -c
	
clean:
	rm -rf $(BIN) $(OBJ)
