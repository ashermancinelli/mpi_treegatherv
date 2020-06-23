
ifeq ($(host_CC),)
host_CC = cc
endif

ifeq ($(BINDIR),)
BINDIR=.
endif

all:
	$(host_CC) $(CFLAGS) bench.c -o $(BINDIR)/bench.bin

check:
	if [ ! $(shell echo '19:37:58.792808359 19:37:55.812240374' | $(BINDIR)/bench.bin) -eq 19429015 ]; then exit 1; fi
	if [ ! $(shell printf '19:37:58.792808359 19:37:55.812240374' | $(BINDIR)/bench.bin) -eq 19429015 ]; then exit 1; fi
	@echo
	@echo Benchmarking utility tests passed
	@echo

clean:
	if [ -d bench.bin ]; then rm bench.bin; fi
