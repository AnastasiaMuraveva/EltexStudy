BIN := main
OBJ := main.o threads_jobs.o
SRC := main.c threads_jobs.c

CC := gcc
CFLAGS := -Wall -g

.PHONY : clean

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)
	
%.o: %.c
	$(CC) $^ $(CFLAGS) $^ -c
	
clean:
	rm -rf $(BIN) $(OBJ)
