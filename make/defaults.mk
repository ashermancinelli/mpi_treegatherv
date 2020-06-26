
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
