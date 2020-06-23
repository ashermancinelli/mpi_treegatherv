
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(OBJ)
	cp utils.h $(INCDIR)

%.o: %.c
	$(CC) 			-c $< 			-o $(BUILDDIR)/$@ 			$(CFLAGS)

clean:

