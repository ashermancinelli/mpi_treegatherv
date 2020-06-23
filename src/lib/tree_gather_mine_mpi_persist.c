#include "tree_gather.h"

int my_mpi_gatherv_persistent(
        double *sendbuf, int sendcnt,   MPI_Datatype sendtype,
        double *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm,
        MPI_Request* reqs)
{
    int i, j, rank, comm_size;

    UNUSED(recvtype);
    UNUSED(sendtype);

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &comm_size);

    if (reqs[0] == MPI_REQUEST_NULL)
    {
        for (i=0; i<MAX_MPI_RANKS; i++)
            reqs[i] = MPI_REQUEST_NULL;

        if (rank == root)
        {
            memcpy(recvbuf + displs[rank], sendbuf, sizeof(double)*sendcnt);
            for (j=0, i=0; i<comm_size; i++)
                if (i != rank)
                    MPI_Recv_init(recvbuf + displs[i], recvcnts[i],
                        MPI_DOUBLE, i, 0, comm, &reqs[j++]);
        }
        else
            MPI_Send_init(sendbuf, sendcnt, MPI_DOUBLE,
                    root, 0, comm, &reqs[0]);
    }

    if (rank != root)
    {
        MPI_Start(&reqs[0]);
        MPI_Wait(&reqs[0], MPI_STATUS_IGNORE);
    }
    else
    {
        j = 0;
        for (i=0; i<MAX_MPI_RANKS; i++)
            if (reqs[i] != MPI_REQUEST_NULL)
                j++;
        MPI_Startall(j, reqs);
        MPI_Waitall(j, reqs, MPI_STATUSES_IGNORE);
    }
    return 0;
}
