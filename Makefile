
include make/defaults.mk
include make/vars.mk
include make/integration_tests.mk

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

clean:
	@rm -rf $(BUILDDIR)
	@$(MAKE) -C src clean
