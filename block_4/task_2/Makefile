BIN := main
OBJ := main.o base.o drivers.o
SRC := main.c list.c drivers.c

CC := gcc
CFLAGS := -g -Wall -Wextra
LDFLAGS := -lrt

.PHONY : clean

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)
	
%.o: %.c
	$(CC) $^ $(CFLAGS) $^ -c
	
clean:
	rm -rf $(BIN) $(OBJ)
