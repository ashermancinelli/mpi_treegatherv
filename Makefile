
ifeq ($(BUILD_TYPE),)
BUILD_TYPE = debug
endif

ifeq ($(PREFIX),)
PREFIX=./install
endif

ifeq ($(USE_LOCAL_MPI),1)
# Use the mpi we build in travis container
export CC 						= $(abspath ./mpich/bin/mpicc)
export host_CC 				= gcc
else
export CC				      = mpicc
export host_CC        = $(shell mpicc -showme | cut -f1 -d' ')
endif

export BUILDDIR	      = $(shell pwd)/build
export BINDIR		      = $(BUILDDIR)/bin
export INCDIR		      = $(BUILDDIR)/include
export BIN_NAME_SHORT = treegather.bin
export BIN_NAME	 			= $(BINDIR)/$(BIN_NAME_SHORT)

export CFLAGS		      =  -Wall
export CFLAGS         += -I$(INCDIR)

ifeq ($(BUILD_TYPE),debug)
export CFLAGS         += -Wextra -Wpedantic -Wmisleading-indentation
export CFLAGS         += -O0
export CFLAGS         += -g
else ifeq ($(BUILD_TYPE),release)
export CFLAGS         += -O3
endif

all: info
	if [ ! -d $(BINDIR) ]; then mkdir -p $(BINDIR); fi
	if [ ! -d $(INCDIR) ]; then mkdir -p $(INCDIR); fi
	$(MAKE) -C src
	$(MAKE) -C tools

info:
	@echo
	@echo BUILD_TYPE $(BUILD_TYPE)
	@echo
	@echo BUILDIDR $(BUILDDIR)
	@echo
	@echo BINDIR $(BINDIR)
	@echo
	@echo INCIDR $(INCDIR)
	@echo
	@echo BIN NAME $(BIN_NAME_SHORT)
	@echo
	@echo CC $(CC)
	@echo
	@echo host_CC $(host_CC)
	@echo
	@echo CFLAGS $(CFLAGS)
	@echo

install: all
	if [ ! -d $(PREFIX)/bin ]; then mkdir -p $(PREFIX)/bin; fi
	if [ ! -d $(PREFIX)/include ]; then mkdir -p $(PREFIX)/include; fi
	if [ ! -z "$(PREFIX)" ]; then install $(BINDIR)/* $(PREFIX)/bin/; fi
	cp $(INCDIR)/* $(PREFIX)/include

check: install
	$(BIN_NAME) --data-per-node 1024 --gather-method mpi 		\
			--num-loops 10 		|| exit 1
	$(BIN_NAME) --data-per-node 1024 --gather-method my-mpi \
			--num-loops 10 		|| exit 1
	$(BIN_NAME) --data-per-node 1024 --gather-method tree 	\
			--num-loops 10 		|| exit 1
	$(BIN_NAME) --data-per-node 1024 --gather-method itree 	\
			--num-loops 10 		|| exit 1
	$(BIN_NAME) --data-per-node 1024 --gather-method mpi 		\
			--num-loops 10 --persist		|| exit 1
	$(BIN_NAME) --data-per-node 1024 --gather-method my-mpi \
			--num-loops 10 --persist		|| exit 1
	$(BIN_NAME) --data-per-node 1024 --gather-method tree 	\
			--num-loops 10 --persist		|| exit 1
	$(BIN_NAME) --data-per-node 1024 --gather-method itree 	\
			--num-loops 10 --persist		|| exit 1
	@echo
	@echo MPI_Gatherv tests passed
	@echo
	make -C tools check

clean:
	rm -rf $(BUILDDIR)
	$(MAKE) -C src clean
