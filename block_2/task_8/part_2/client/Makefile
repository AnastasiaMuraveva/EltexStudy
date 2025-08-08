BIN := client
OBJ := client.o user_list.o history_list.o graphics.o
SRC := client.c user_list.c history_list.c graphics.c

CC := gcc
CFLAGS := -g -Wall -Wextra
LDFLAGS := -lrt -lncurses

.PHONY : clean

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)
	
%.o: %.c
	$(CC) $^ $(CFLAGS) $^ -c
	
clean:
	rm -rf $(BIN) $(OBJ)
	
