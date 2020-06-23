#include "tree_gather.h"

int my_mpi_gatherv(
        double *sendbuf, int sendcnt,   MPI_Datatype sendtype,
        double *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm)
{
    int i, j, rank, comm_size;
    MPI_Request reqs[MAX_MPI_RANKS];

    UNUSED(recvtype);
    UNUSED(sendtype);

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &comm_size);

    if (rank == root)
    {
        memcpy(recvbuf + displs[rank], sendbuf, sizeof(double)*sendcnt);
        j = 0;
        for (i=0; i<comm_size; i++)
        {
            reqs[j] = MPI_REQUEST_NULL;
            if (i != rank)
                MPI_Irecv(recvbuf + displs[i], recvcnts[i],
                        MPI_DOUBLE, i, 0, comm, &reqs[j++]);
        }
        MPI_Waitall(j, reqs, MPI_STATUSES_IGNORE);
    }
    else
        MPI_Send(sendbuf, sendcnt, MPI_DOUBLE, root, 0, comm);

    return 0;
}
