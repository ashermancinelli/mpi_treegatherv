
.DEFAULT_GOAL := all

include make/defaults.mk
include make/vars.mk

all: info
	for dir in $(BUILDDIR) $(BINDIR) $(INCDIR) $(LIBDIR); do if [ ! -d $$dir ]; then mkdir -p $$dir; fi done
	$(MAKE) -C src
	$(MAKE) -C tools

include make/integration_tests.mk

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
	@echo
	@echo Setting up install directory in $(PREFIX)
	@echo
	if [ ! -d $(PREFIX)/bin ]; then mkdir -p $(PREFIX)/bin; fi
	if [ ! -d $(PREFIX)/include/treegatherv ]; then mkdir -p $(PREFIX)/include/treegatherv; fi
	if [ ! -d $(PREFIX)/lib ]; then mkdir -p $(PREFIX)/lib; fi
	install $(BUILDDIR)/bin/$(BIN_NAME_SHORT).bin $(PREFIX)/bin/
	install $(BUILDDIR)/bin/bench.bin $(PREFIX)/bin/
	if [ ! -z "$(MAKE_SHARED)" ]; then \
		$(CC) $(SHAREDFLAGS) $(BUILDDIR)/*.o; \
	fi
	ar cr "$(BUILDDIR)/lib/$(BIN_NAME_SHORT).a" $(BUILDDIR)/*.o
	cp $(INCDIR)/* $(PREFIX)/include/treegatherv
	cp $(BUILDDIR)/lib/* $(PREFIX)/lib/
	
	

clean:
	rm -rf $(BUILDDIR)
	$(MAKE) -C src clean
