
export CC				 = mpicc #$(which mpicc)
export BUILDDIR	 = $(shell pwd)/build
export BINDIR		 = $(BUILDDIR)/bin
export INCDIR		 = $(BUILDDIR)/include
export BIN_NAME_SHORT = treegather.bin
export BIN_NAME	 = $(BINDIR)/$(BIN_NAME_SHORT)

export CFLAGS		 = -Wall
export CFLAGS    += -Wextra
export CFLAGS    += -Wpedantic
export CFLAGS    += -O0
export CFLAGS    += -g
export CFLAGS    += -I$(INCDIR)

all:
	@echo
	@echo Building in $(BUILDDIR)
	@echo
	if [ ! -d $(BINDIR) ]; then mkdir -p $(BINDIR); fi
	if [ ! -d $(INCDIR) ]; then mkdir -p $(INCDIR); fi
	$(MAKE) -C src

install: all
	if [ ! -d $(PREFIX)/bin ]; then mkdir -p $(PREFIX)/bin; fi
	if [ ! -z "$(PREFIX)" ]; then install $(BIN_NAME) $(PREFIX)/bin/$(BIN_NAME_SHORT); fi

clean:
	rm -rf $(BUILDDIR)
	$(MAKE) -C src clean
