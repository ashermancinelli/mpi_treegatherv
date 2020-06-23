
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(OBJ)
	cp *.h $(INCDIR)

%.o: %.c
	$(CC) 			-c $< 		$(CFLAGS) 			-o $(BUILDDIR)/$@

clean:

check: all
	$(MAKE) -C tests
