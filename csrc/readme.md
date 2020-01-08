## MPI Tree-Based Gatherv

### Hypotheses

#### Hypothesis 1

The height of the tree in the tree-based method will be
ceil(log_2(p)) where p is the number of processors that the 
gather will be spread over. We therefore expect that the
performance gain from the tree-based method will grow with
the number of processors used.

MPI Implementations also use shared memory between ranks on the
same machine, so the gather will probably need to be ran on 
more than one node to see performance increases. We might hypothesize
that with world size of 144 (and therefore a tree size of 8) we may see
a 144/8 fold increase in performance, though this is doubtful.

#### Hypothesis 2

We expect a knee in the performance curve as the number of
gatherv calls increase.

### Parameters Used
data sizes to be tested:

- 2 cores on 1 node (addjust number of gatherv calls to make this > 5 min)
- 2 cores on 2 nodes
- 4 cores on 1 node
- 4 cores on 2 nodes
- 4 cores on 4 nodes
- 8 cores on 1 node
- 8 cores on 2 nodes
- 8 cores on 4 nodes
- 12 cores on 1 node
- 12 cores on 2 nodes
- 12 cores on 4 nodes
- 16 cores on 1 node
- 16 cores on 2 nodes
- 16 cores on 4 nodes
- 24 cores on 1 node
- 24 cores on 2 nodes
- 24 cores on 4 nodes
- 24 cores on 6 nodes
- 36 cores on 2 nodes
- 36 cores on 4 nodes
- 48 cores on 2 nodes
- 48 cores on 4 nodes
- 48 cores on 6 nodes
- 72 cores on 4 nodes
- 72 cores on 6 nodes
- 96 cores on 4 nodes
- 96 cores on 6 nodes
- 144 cores on 6 nodes

Message sizes to be tested:

- .5M   (2^19)
- 1M    (2^20)
- 2M    (2^21)
- 4M    (2^22)

Styles of communication to be tested:

- Native MPI Gatherv
- blocking tree-based
- non-blocking tree-based
- persistent communication tree-based

MPI Versions:

- Open MPI
- Intel MPI
- MVAPICH2
