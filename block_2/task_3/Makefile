BIN := main
OBJ := main.o 
SRC := main.c 

CC := gcc
CFLAGS := -g -Wall
LDFLAGS := -L. -lcalc

.PHONY : clean

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)
	
%.o: %.c
	$(CC) $^ $(CFLAGS) $^ -c
	
clean:
	rm -rf $(BIN) $(OBJ)
