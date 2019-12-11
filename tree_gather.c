
#include "tree_gather.h"

/*
 * Ignored: sendtype, recvtype
 *
 * Only using floats for now.
 *
 * See header for algorithm explanation.
 * Using this for reference: http://formalverification.cs.utah.edu/sawaya/html/d0/d70/mpi_2coll_2gather_8c-source.html
 *
 */
void tree_gatherv_f(
        float *sendbuf, int sendcnt,   MPI_Datatype sendtype,
        float *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm
    ) {
    int i, rank, comm_size, bits=0;

    UNUSED(sendtype);
    UNUSED(recvtype);

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &comm_size);

    int tmp_size = comm_size;
    while (tmp_size)
    {
        bits++;
        tmp_size >>= 1;
    }

    if (rank == root)
    {
        fprintf(stdout, "Bits to hold world size: %i\n"
            "Highest rank: %i\n"
            "Offset from highest rank to "
            "highest capacity for number of bits: %i\n",
            bits, 1<<bits, (1<<bits)-comm_size);
    }

    for (i=0; i<comm_size; i++)
    {
        MPI_Send(sendbuf, sendcnt, MPI_FLOAT, root, 0, comm);
    }

    if (rank == root)
    {
        for (i=0; i<comm_size; i++)
        {
            MPI_Recv(recvbuf + displs[i], recvcnts[i], MPI_FLOAT, i, 0, comm, MPI_STATUS_IGNORE);
        }
    }

    /*
    MPI_Gatherv(
        sendbuf,
        sendcnt,
        sendtype,
        recvbuf,
        recvcnts,
        displs,
        recvtype,
        root,
        comm
    );
    */
}
