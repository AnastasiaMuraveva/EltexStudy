BIN := server
OBJ := server.o server_func.o
SRC := server.c server_func.c

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
