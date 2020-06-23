
export CC				 = mpicc #$(which mpicc)
export BUILDDIR	 = $(shell pwd)/build
export BINDIR		 = $(BUILDDIR)/bin
export INCDIR		 = $(BUILDDIR)/include
export BIN_NAME	 = $(BINDIR)/treegather.bin
export CFLAGS		 = -Wall
export CFLAGS += -Wextra
export CFLAGS += -Wpedantic
export CFLAGS += -O0
export CFLAGS += -g
export CFLAGS += -I$(INCDIR)

all:
	@echo
	@echo Building in $(BUILDDIR)
	@echo
	if [ ! -d $(BINDIR) ]; then mkdir -p $(BINDIR); fi
	if [ ! -d $(INCDIR) ]; then mkdir -p $(INCDIR); fi
	$(MAKE) -C src

clean:
	rm -rf $(BUILDDIR)
	$(MAKE) -C src clean
