# mpi_treegatherv

A tree-based gathering function to reduce cost of a gatherv call

## Dependencies

So far just tested on:
- Red Hat Enterprise Linux Workstation release 6.6 (Santiago)
- GCC       >= 7.3.0
- OpenMPI   >= 3.1.0

## Testing

Results of testing the synchronous tree-based algorithm,
`tree_gatherv_d`, showed minor
improvements, but would need more rigerous testing to ensure a 
worthwhile speedup.

The asynchronous version of the gatherv function however, `tree_gatherv_d_async`,
showed very significant speedups. For example:

```bash
> # using 384 doubles per proc
> mpirun -np 24 ./treegather.bin --gather-method mpi
time mpirun -np 24 ./treegather.bin --gather-method mpi

Using gather method: mpi.
mpirun -np 24 ./treegather.bin --gather-method mpi  0.48s user 0.46s system 1346% cpu 0.070 total
> mpirun -np 24 ./treegather.bin --gather-method tree
time mpirun -np 24 ./treegather.bin --gather-method tree

Using gather method: tree.
mpirun -np 24 ./treegather.bin --gather-method tree  1.10s user 0.49s system 1482% cpu 0.107 total
> mpirun -np 24 ./treegather.bin --gather-method itree
time mpirun -np 24 ./treegather.bin --gather-method itree

Using gather method: itree.
mpirun -np 24 ./treegather.bin --gather-method itree  0.17s user 0.52s system 1091% cpu 0.064 total
```

As we can see, using the `itree` parameter, which calls `tree_gatherv_d_async`, runs
__much__, __much__ faster than any of the other algorithms.

