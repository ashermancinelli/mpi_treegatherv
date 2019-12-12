
CC = mpicc
SRC = $(wildcard *.c)
OBJ = $(SRC:.o=.c)
HEADERS = $(wildcard *.h)
CFLAGS_BASE = -Wall -Wextra -lmpi
CFLAGS_DEBUG = $(CFLAGS_BASE) -D __DEBUG -g
CFLAGS_OPT = $(CFLAGS_BASE) -O3
BIN_NAME = treegather.bin

.PHONY: Makfile all

all: $(OBJ)
	$(CC) $(CFLAGS_DEBUG) -o $(BIN_NAME) $(OBJ)

opt: $(OBJ)
	$(CC) $(CFLAGS_OPT) -o $(BIN_NAME) $(OBJ)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean all run

run: all
	#MALLOC_CHECK_=2 \
				  
	mpirun -np 15 treegather.bin --gather-method itree

clean:
	if [ -f main.o ]; then rm *.o; fi
	if [ -f $(BIN_NAME) ]; then rm $(BIN_NAME); fi
