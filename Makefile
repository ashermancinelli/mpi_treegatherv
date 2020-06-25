
ifeq ($(BUILD_TYPE),)
export BUILD_TYPE 		= debug
endif

ifeq ($(PREFIX),)
export PREFIX					= $(shell pwd)/install
endif

export BUILDDIR	      = $(shell pwd)/build
export BINDIR		      = $(BUILDDIR)/bin
export INCDIR		      = $(BUILDDIR)/include
export BIN_NAME_SHORT = treegather.bin
export BIN_NAME	 			= $(BINDIR)/$(BIN_NAME_SHORT)
export MPIRUN_ARGS		= -np 2

export CFLAGS         = -I$(INCDIR)

ifeq ($(BUILD_TYPE),debug)
export CFLAGS         += -Wall -Wextra -Wpedantic -Wmisleading-indentation
export CFLAGS         += -O0 -g
else ifeq ($(BUILD_TYPE),release)
export CFLAGS         += -O3 -DRELEASE
endif

ifeq ($(USE_BARRIERS),1)
export CFLAGS					+= -DUSE_BARRIERS
endif

ifeq ($(VERBOSE_OUTPUT),1)
export CFLAGS					+= -DVERBOSE_OUTPUT
endif

ifeq ($(USE_LOCAL_MPI),1)
# Use the mpi we build in travis container
export CC 						= $(abspath ./mpich/bin/mpicc)
export HOSTCC 				= gcc
export CFLAGS					+= -I$(abspath ./mpich/include) -L$(abspath ./mpich/lib) -Wl,-rpath=$(abspath ./mpich/lib)
export MPIRUN					= $(abspath ./mpich/bin/mpirun)
else
export CC				      = mpicc
export HOSTCC         = $(shell mpicc -showme | cut -f1 -d' ')
export MPIRUN					= mpirun
endif

# Use absolute paths
export CC							:= $(shell which $(CC))
export HOSTCC					:= $(shell which $(HOSTCC))

all: info
	for dir in $(BUILDDIR) $(BINDIR) $(INCDIR); do \
		if [ ! -d $$dir ]; then mkdir -p $$dir; fi \
	done
	$(MAKE) -C src
	$(MAKE) -C tools

info:
	@printf 'BUILD_TYPE\t\t= $(BUILD_TYPE)'
	@echo
	@printf 'BUILDIDR\t\t= $(BUILDDIR)'
	@echo
	@printf 'PREFIX\t\t\t= $(PREFIX)'
	@echo
	@printf 'BINDIR\t\t\t= $(BINDIR)'
	@echo
	@printf 'BIN\t\t\t= $(BIN_NAME)'
	@echo
	@printf 'INCIDR\t\t\t= $(INCDIR)'
	@echo
	@printf 'BINNAME\t\t\t= $(BIN_NAME_SHORT)'
	@echo
	@printf 'CC\t\t\t= $(CC)'
	@echo
	@printf 'HOSTCC\t\t\t= $(HOSTCC)'
	@echo
	@printf 'CFLAGS\t\t\t= $(CFLAGS)'
	@echo
	@printf 'VERBOSE_OUTPUT\t\t= $(VERBOSE_OUTPUT)'
	@echo

install: all
	if [ ! -d $(PREFIX)/bin ]; then mkdir -p $(PREFIX)/bin; fi
	if [ ! -d $(PREFIX)/include ]; then mkdir -p $(PREFIX)/include; fi
	install $(BUILDDIR)/bin/treegather.bin $(PREFIX)/bin/
	install $(BUILDDIR)/bin/bench.bin $(PREFIX)/bin/
	cp $(INCDIR)/* $(PREFIX)/include

check: all
	@echo
	@echo Gather Algorithms Integration Tests:
	@echo
	$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method mpi 		\
			--num-loops 10 		|| exit 1
	$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method my-mpi \
			--num-loops 10 		|| exit 1
	$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method tree 	\
			--num-loops 10 		|| exit 1
	$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method itree 	\
			--num-loops 10 		|| exit 1
	$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method my-mpi \
			--num-loops 10 --persist		|| exit 1
	$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method itree 	\
			--num-loops 10 --persist		|| exit 1
	@echo
	@echo Gather Algorithms Integration Tests passed
	@echo
	$(MAKE) -C src check
	$(MAKE) -C tools check

clean:
	rm -rf $(BUILDDIR)
	$(MAKE) -C src clean
