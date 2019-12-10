
CC = mpicc
SRC = $(wildcard *.c)
OBJ = $(SRC:.o=.c)
HEADERS = $(wildcard *.h)
CFLAGS = -O3 -Wall -Wextra -lmpi
BIN_NAME = treegather.bin

.PHONY: Makfile all

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN_NAME) $(OBJ)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean all

clean:
	if [ -f main.o ]; then rm *.o; fi
	if [ -f $(BIN_NAME) ]; then rm $(BIN_NAME); fi
