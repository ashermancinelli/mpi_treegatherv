
.PHONY: Makefile clean all run

all:
	$(MAKE) -C common # Common must be compiled first; has headers needed by all
	$(MAKE) -C utils # Lib uses some utils functions
	$(MAKE) -C lib # Lib contains the kernels
	$(MAKE) -C driver

clean:
	$(MAKE) -C lib clean
	$(MAKE) -C utils clean
	$(MAKE) -C driver clean
