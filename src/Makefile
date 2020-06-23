
all:
	$(MAKE) -C common # Common must be compiled first; has headers needed by all
	$(MAKE) -C utils # Lib uses some utils functions
	$(MAKE) -C lib # Lib contains the kernels
	$(MAKE) -C driver

check: all
	$(MAKE) -C lib check
	$(MAKE) -C utils check

clean:
	$(MAKE) -C lib clean
	$(MAKE) -C utils clean
	$(MAKE) -C driver clean
