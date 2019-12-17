#ifndef TREE_REDUCE_H
#define TREE_REDUCE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "mpi.h"

#define UNUSED(x) (void)(x)
#define MAX_MPI_RANKS 50
#define GREEN       "\033[0;32m"
#define RED         "\033[1;31m"
#define BLUE        "\033[1;34m"
#define RESET       "\033[0m"

/*
 * Same function stub as MPI_Gatherv
 * Should function the exact same, but
 * with a performance boost from the tree
 * structure
 */
extern void tree_gatherv_d(
        double *sendbuf, int sendcnt, MPI_Datatype sendtype,
        double *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm
    );

extern void tree_gatherv_d_async(
        double *sendbuf, int sendcnt, MPI_Datatype sendtype,
        double *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm
    );

extern void my_mpi_gatherv(
        double *sendbuf, int sendcnt,   MPI_Datatype sendtype,
        double *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm
    );
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
