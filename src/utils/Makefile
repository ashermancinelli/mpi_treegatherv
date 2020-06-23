
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(OBJ)
	@echo
	cp utils.h $(INCDIR)

%.o: %.c
	@echo
	$(CC) 			-c $< 			-o $(BUILDDIR)/$@ 			$(CFLAGS)

clean:

check:
	$(MAKE) -C tests
