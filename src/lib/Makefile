
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(OBJ)
	@echo
	cp *.h $(INCDIR)

%.o: %.c
	@echo
	$(CC) 			-c $< 		$(CFLAGS) 			-o $(BUILDDIR)/$@

clean:

check: all
	$(MAKE) -C tests
