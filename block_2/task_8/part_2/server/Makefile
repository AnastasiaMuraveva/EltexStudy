BIN := server
OBJ := server.o user_list.o history_list.o
SRC := server.c user_list.c history_list.c

CC := gcc
CFLAGS := -g
LDFLAGS := -lrt

.PHONY : clean

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)
	
%.o: %.c
	$(CC) $^ $(CFLAGS) $^ -c
	
clean:
	rm -rf $(BIN) $(OBJ)
	
