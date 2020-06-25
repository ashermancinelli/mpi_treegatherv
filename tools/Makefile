
ifeq ($(HOSTCC),)
HOSTCC = cc
endif

ifeq ($(BINDIR),)
BINDIR=$(shell pwd)
endif

all:
	@echo
	$(HOSTCC) $(CFLAGS) bench.c -o $(BINDIR)/bench.bin

check:
	if [ ! $(shell echo '10:37:20.170391791 10:37:19.687822065' | $(BINDIR)/bench.bin) -eq 482569726 ]; then exit 1; fi
	if [ ! $(shell printf '10:37:20.170391791 10:37:19.687822065' | $(BINDIR)/bench.bin) -eq 482569726  ]; then exit 1; fi
	@echo
	@echo Benchmarking utility tests passed
	@echo

clean:
	if [ -d bench.bin ]; then rm bench.bin; fi
