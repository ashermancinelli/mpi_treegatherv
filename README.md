Tree-based MPI GatherV implementation
---

[![Build Status](https://travis-ci.org/ashermancinelli/mpi_treegatherv.svg?branch=master)](https://travis-ci.org/ashermancinelli/mpi_treegatherv)

A tree-based gathering function to reduce cost of a gatherv call

## Dependencies

So far just tested on:
- Red Hat Enterprise Linux Workstation release 6.6 (Santiago)
- GCC       >= 7.3.0
- OpenMPI   >= 3.1.0

## Installation

```console
$ make check
$ make install PREFIX=/some/dir
```

## Testing

Results of testing the synchronous tree-based algorithm,
`tree_gatherv_d`, showed minor
improvements, but would need more rigerous testing to ensure a 
worthwhile speedup.

The asynchronous version of the gatherv function however, `tree_gatherv_d_async`,
showed very significant speedups.

To see tests, run `make check`.

