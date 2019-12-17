
CC = mpicc
SRC = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJ = $(SRC:.o=.c)
CFLAGS = -Wall -Wextra -g
BIN_NAME = treegather.bin

.PHONY: Makefile clean all run

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN_NAME) $^

%.o: %.c $(HEADERS)
	$(CC) $(FFLAGS) -c $< -o $@

clean:
	rm *.o
