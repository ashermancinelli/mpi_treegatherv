
ifeq ($(BUILD_TYPE),)
export BUILD_TYPE 		= debug
endif

ifeq ($(PREFIX),)
export PREFIX					= $(shell pwd)/install
endif

export BUILDDIR	      = $(shell pwd)/build
export BINDIR		      = $(BUILDDIR)/bin
export INCDIR		      = $(BUILDDIR)/include
export LIBDIR		      = $(BUILDDIR)/lib
export BIN_NAME_SHORT = treegather
export BIN_NAME	 			= $(BINDIR)/$(BIN_NAME_SHORT).bin
export MPIRUN_ARGS		= -np 2

export CFLAGS         = -I$(INCDIR)
export PUBLIC_HEADER 	= $(INCDIR)/treegatherv.h
