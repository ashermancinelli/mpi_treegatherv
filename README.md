Tree-based MPI GatherV implementation
---

[![Build Status](https://travis-ci.org/ashermancinelli/mpi_treegatherv.svg?branch=master)](https://travis-ci.org/ashermancinelli/mpi_treegatherv)

A tree-based gathering function to reduce cost of a gatherv call. Please find the readme in the source directory, and the header in `src/lib/tree_gather.h` for more information about the algorithm.

## Dependencies

So far just tested on:
- Red Hat Enterprise Linux Workstation release 6.6 (Santiago)
- GCC       >= 7.3.0
- OpenMPI   >= 3.1.0

## Installation

```console
$ export BUILD_TYPE=release MAKE_SHARED=1 PREFIX=/path/to/installation
$ make -j4 check install
```

This should give you an installation with the public header, shared and static libraries, and the optimized binary both for the benchmarking utility and the driver:

```console
$ ls -R install/
install/:
bin  include  lib

install/bin:
bench.bin  treegather.bin

install/include:
treegatherv.h

install/lib:
treegather.a  treegather.so
```

## Testing

You may run the full suite of unit tests and integration tests with `make check`, or you may run the driver directly.
The driver has the following options:

```console
$ ./build/bin/treegather.bin --help
Usage:
    --gather-method      (mpi|my-mpi|tree|itree)
    --output-file        <filename or blank for stdout>
    --display-buf        Display buffers before/after run
    --data-per-node      <int>
    --num-loops          <int>
    --persistent         Use persistent communication
```

Please note that all utilities are disabled when building in release mode.
You will not be able to get the help message when building in release mode, so you'll have to play with the options in debug mode first.
