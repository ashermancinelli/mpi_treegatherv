
ifeq ($(BUILD_TYPE),debug)
export CFLAGS         += -Wall -Wextra -Wpedantic -Wmisleading-indentation
export CFLAGS         += -O0 -g
else ifeq ($(BUILD_TYPE),release)
export CFLAGS         += -O3 -DRELEASE
endif

ifeq ($(USE_BARRIERS),1)
export CFLAGS					+= -DUSE_BARRIERS
endif

ifeq ($(VERBOSE_OUTPUT),1)
export CFLAGS					+= -DVERBOSE_OUTPUT
endif

ifeq ($(USE_LOCAL_MPI),1)
# Use the mpi we build in travis container
export CC 						= $(abspath ./mpich/bin/mpicc)
export HOSTCC 				= gcc
export CFLAGS					+= -I$(abspath ./mpich/include) -L$(abspath ./mpich/lib) -Wl,-rpath=$(abspath ./mpich/lib)
export MPIRUN					= $(abspath ./mpich/bin/mpirun)
else
export CC				      = mpicc
export HOSTCC         = $(shell mpicc -showme | cut -f1 -d' ')
export MPIRUN					= mpirun
endif

# Use absolute paths
export CC							:= $(shell which $(CC))
export HOSTCC					:= $(shell which $(HOSTCC))
