BIN := main
OBJ := main.o
SRC := main.c

CC := gcc
CFLAGS := -g

.PHONY : clean

$(BIN): $(OBJ)
	$(CC) $^ -o $@
	
%.o: %.c
	$(CC) $^ $(CFLAGS) $^ -c
	
clean:
	rm -rf $(BIN) $(OBJ)
	
$(BIN): $(HDR)
