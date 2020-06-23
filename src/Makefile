
.PHONY: Makefile clean all run

all:
	@echo Building in $(BUILDDIR)
	# Must be first so that common headers will be
	# available to the rest of the compilation units
	$(MAKE) -C common
	$(MAKE) -C utils
	$(MAKE) -C lib
	$(MAKE) -C driver

clean:
	$(MAKE) -C lib clean
	$(MAKE) -C utils clean
	$(MAKE) -C driver clean
