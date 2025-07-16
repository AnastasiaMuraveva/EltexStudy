BIN := main
OBJ := main.o file_system_commands.o
SRC := main.c file_system_commands.c
HDR := file_system_commands.h

CC := gcc
CFLAGS := -g
LDFLAGS := -lncurses

.PHONY : clean

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)
	
%.o: %.c
	$(CC) $^ $(CFLAGS) $^ -c
	
clean:
	rm -rf $(BIN) $(OBJ)
	
$(BIN): $(HDR)
