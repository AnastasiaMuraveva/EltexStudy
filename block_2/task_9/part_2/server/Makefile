BIN := server
OBJ := server.o lists.o
SRC := server.c lists.c

CC := gcc
CFLAGS := -g
LDFLAGS := -lrt -pthread

.PHONY : clean

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)
	
%.o: %.c
	$(CC) $^ $(CFLAGS) $^ -c
	
clean:
	rm -rf $(BIN) $(OBJ)
	
