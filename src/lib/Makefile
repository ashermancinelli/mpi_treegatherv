
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(OBJ)
	cp *.o $(BUILDDIR)
	cp *.h $(INCDIR)

%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm $(OBJ)
