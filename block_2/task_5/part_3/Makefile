BIN := main
OBJ := main.o operations.o
SRC := main.c operations.c
HDR := operations.h

CC := gcc
CFLAGS := -Wall -g

.PHONY : clean

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)
	
%.o: %.c
	$(CC) $^ $(CFLAGS) $^ -c
	
clean:
	rm -rf $(BIN) $(OBJ)
	
$(BIN): $(HDR)
