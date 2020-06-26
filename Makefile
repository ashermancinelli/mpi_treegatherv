
include make/defaults.mk
include make/vars.mk
include make/integration_tests.mk

all: info
	@for dir in $(BUILDDIR) $(BINDIR) $(INCDIR) $(LIBDIR); do if [ ! -d $$dir ]; then mkdir -p $$dir; fi done
	@$(MAKE) -C src
	@$(MAKE) -C tools

info:
	@printf 'BUILD_TYPE\t\t= $(BUILD_TYPE)'
	@echo
	@printf 'BUILDDIR\t\t= $(BUILDDIR)'
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
	@if [ ! -d $(PREFIX)/lib ]; then mkdir -p $(PREFIX)/lib; fi
	install $(BUILDDIR)/bin/treegather.bin $(PREFIX)/bin/
	install $(BUILDDIR)/bin/bench.bin $(PREFIX)/bin/
	@if [ ! -z "$(MAKE_SHARED)" ]; then \
		$(CC) $(SHAREDFLAGS) $(BUILDDIR)/tree_async_persistent.o; \
	fi
	echo "#ifndef _MPI_TREEGATHERV_H_" > $(PUBLIC_HEADER)
	echo "#define _MPI_TREEGATHERV_H_" >> $(PUBLIC_HEADER)
	awk '/tree_async_persistent_gatherv/{while(index($$0, ")")==0){print; getline;}print}' \
		$(INCDIR)/tree_gather.h >> $(PUBLIC_HEADER)
	echo "#endif" >> $(PUBLIC_HEADER)
	ar cr "$(BUILDDIR)/lib/$(BIN_NAME_SHORT).a" $(BUILDDIR)/tree_async_persistent.o
	cp $(PUBLIC_HEADER) $(PREFIX)/include
	cp $(BUILDDIR)/lib/* $(PREFIX)/lib/
	
	

clean:
	@rm -rf $(BUILDDIR)
	@$(MAKE) -C src clean
