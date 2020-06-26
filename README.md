Tree-based MPI GatherV implementation
---

[![Build Status](https://travis-ci.org/ashermancinelli/mpi_treegatherv.svg?branch=master)](https://travis-ci.org/ashermancinelli/mpi_treegatherv)

A tree-based gathering function to reduce cost of a gatherv call. Please find the readme in the source directory, and the header in `src/lib/tree_gather.h` for more information about the algorithm.

## Dependencies

So far just tested on:
- OS
    - Red Hat Enterprise Linux Workstation release 6.6 (Santiago)
    - Red Hat Enterprise Linux Server release 7.6 (Santiago)
    - Ubuntu 18.04.4 LTS (Bionic Beaver)
    - CentOS Linux 7 (Core)
    - OSX Catalina 10.15.5
- Arch
    - IBM Power9 ppc64le
    - x86_64
- Compiler
    - GCC
        - 7.1.0
        - 7.3.0
        - 7.4.0
        - 7.5.0
        - 8.1.0
    - Intel
        - 17.0.1
        - 17.0.4
        - 18.0.0
        - 19.0.3
- MPI
    - OpenMPI
        - 3.0.1
        - 3.1.0
        - 3.1.3
        - 3.1.5
        - 4.0.1
    - Intel
        - 4.1.3.048
        - 5.0.1.035
        - 5.0.2.044
        - 5.1.3.181

## Installation

For an optimized build with no help messages in the binaries:
```console
$ make BUILD_TYPE=release PREFIX=/path/to/installation -j4 check install
```

For an unoptimized debug build with a helpful CLI:
```console
$ make BUILD_TYPE=debug PREFIX=/path/to/installation -j4 check install
```
You may also set `VERBOSE_OUTPUT=1` for extra debugging information.

For a full installation with the public header, shared and static libraries, and the optimized binary both for the benchmarking utility and the driver:
```console
$ make BUILD_TYPE=release PREFIX=$(pwd)/install MAKE_SHARED=1 -j4 check install
$ ls -R install/
install/:
bin  include  lib

install/bin:
bench.bin  treegatherv.bin

install/include:
treegatherv.h

install/lib:
treegatherv.a  treegatherv.so
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
