#ifndef _TREE_GATHER_H_
#define _TREE_GATHER_H_

#include "common.h"
#include "utils.h"

#define UNUSED(x) (void)(x)
#define MAX_MPI_BITS    10
#define MAX_MPI_RANKS   70

/*
 * Same function stub as MPI_Gatherv
 * Should function the exact same, but
 * with a performance boost from the tree
 * structure
 */
extern int tree_gatherv_d(
        double *sendbuf, int sendcnt, MPI_Datatype sendtype,
        double *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm
    );

extern int tree_gatherv_d_async(
        double *sendbuf, int sendcnt, MPI_Datatype sendtype,
        double *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm
    );

extern int tree_gatherv_d_persistent(
        double *sendbuf, int sendcnt, MPI_Datatype sendtype,
        double *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm,
        MPI_Request* reqs
    );

extern int my_mpi_gatherv(
        double *sendbuf, int sendcnt,   MPI_Datatype sendtype,
        double *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm
    );

extern int my_mpi_gatherv_persistent(
        double *sendbuf, int sendcnt,   MPI_Datatype sendtype,
        double *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm,
        MPI_Request* reqs
    );

inline __attribute__((always_inline))
unsigned int num_bits(int n)
{
    int count = 0;
    while (n)
    {
        n >>= 1;
        count++;
    }
    return count;
}

inline __attribute__((always_inline))
unsigned int sum(int* ar, int count)
{
    int n = 0, i;
    for (i=0; i<count; i++)
        n += ar[i];
    return n;
}

// rank of rightmost child
inline int min_child_rank(
        int* children,
        int len)
{
    int i, min=INT_MAX;
    for (i=0; i<len; i++)
        min = children[i] < min ? children[i] : min;
    return min;
}

// rank of leftmost child
inline int max_child_rank(
        int* children,
        int len)
{
    int i, min=0;
    for (i=0; i<len; i++)
        min = children[i] > min ? children[i] : min;
    return min;
}

#if 0

Tree-based reduce function

Reduces nodes by flipping higher and higher bits until
all elements of the vector are reduced down. For a given
set of processes who each have a piece of the vector.
The 1 << i th bit is flipped to find its partner, and the
lower of the two partners recieves the data:

0  000
      -> 000
1  001
              -> 000
2  010
      -> 010
3  011
                      -> 000
4  100
      -> 100
5  101
              -> 100
6  110
      -> 110
7  111

This way the root node is not throttled, but still recieves
all the data at the end of the operation.
#endif

#endif
