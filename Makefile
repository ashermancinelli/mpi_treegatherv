
include make/defaults.mk
include make/vars.mk

all: info
	@for dir in $(BUILDDIR) $(BINDIR) $(INCDIR); do \
		if [ ! -d $$dir ]; then mkdir -p $$dir; fi \
	done
	@$(MAKE) -C src
	@$(MAKE) -C tools

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
	@if [ ! -d $(PREFIX)/bin ]; then mkdir -p $(PREFIX)/bin; fi
	@if [ ! -d $(PREFIX)/include ]; then mkdir -p $(PREFIX)/include; fi
	@install $(BUILDDIR)/bin/treegather.bin $(PREFIX)/bin/
	@install $(BUILDDIR)/bin/bench.bin $(PREFIX)/bin/
	@cp $(INCDIR)/* $(PREFIX)/include

check: all
	@echo
	@echo Gather Algorithms Integration Tests:
	@echo
	@$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method mpi 		\
			--num-loops 10 		|| exit 1
	@echo
	@echo Test mpi: pass
	@echo
	@$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method my-mpi \
			--num-loops 10 		|| exit 1
	@echo
	@echo Test my mpi: pass
	@echo
	@$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method tree 	\
			--num-loops 10 		|| exit 1
	@echo
	@echo Test tree: pass
	@echo
	@$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method itree 	\
			--num-loops 10 		|| exit 1
	@echo
	@echo Test itree: pass
	@echo
	@$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method my-mpi \
			--num-loops 10 --persist		|| exit 1
	@echo
	@echo Test my mpi persist: pass
	@echo
	@$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method itree 	\
			--num-loops 10 --persist		|| exit 1
	@echo
	@echo Test itree persist: pass
	@echo
	@echo
	@echo Gather Algorithms Integration Tests passed
	@echo
	@$(MAKE) -C src check
	@$(MAKE) -C tools check

clean:
	@rm -rf $(BUILDDIR)
	@$(MAKE) -C src clean
