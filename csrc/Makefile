
CC = mpicc
SRC = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJ = $(SRC:.o=.c)
CFLAGS = -Wall -Wextra -g -D__DEBUG
CFLAGS_OPT = -O3
BIN_NAME = treegather.bin

.PHONY: Makefile clean all run

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN_NAME) $^

opt: $(OBJ)
	$(CC) $(CFLAGS_OPT) -o $(BIN_NAME) $^

%.o: %.c $(HEADERS)
	$(CC) $(FFLAGS) -c $< -o $@

clean:
	[ -f $(BIN_NAME) ] && rm $(BIN_NAME)
	[ -f main.o ] && rm *.o
