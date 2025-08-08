BIN := client
OBJ := client.o lists.o graphics.o
SRC := client.c lists.c graphics.c

CC := gcc
# CFLAGS := -fsanitize=address,leak,undefined -fno-common -fno-omit-frame-pointer -g -Wall -Wextra
# LDFLAGS := -fsanitize=address,leak,undefined -lrt -lncurses

CFLAGS := -g -Wall -Wextra
LDFLAGS := -lrt -lncurses

.PHONY : clean

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)
	
%.o: %.c
	$(CC) $^ $(CFLAGS) $^ -c
	
clean:
	rm -rf $(BIN) $(OBJ)
	
