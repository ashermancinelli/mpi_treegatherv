#include "tree_gather.h"

/*
 * parameter reqs must have allocated enough space
 * to hold teh correct number of requests
 */
int tree_gatherv_d_persistent(
    double *sendbuf, int sendcnt, MPI_Datatype sendtype,
    double *recvbuf, int *recvcnts, int *displs,
    MPI_Datatype recvtype, int root, MPI_Comm comm,
    MPI_Request* reqs)
{
  int i, rank, comm_size, bits=0, partner_rank;
  int num_reqs = 0;

  UNUSED(sendtype);
  UNUSED(recvtype);

  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &comm_size);

  if (recvbuf == NULL)
    recvbuf = sendbuf;
  else
    memcpy(recvbuf + displs[rank], sendbuf, sizeof(double)*sendcnt);

  bits = num_bits(comm_size);

#ifndef RELEASE
  fprintf(stdout, "%s", BLUE);
  if (rank == root)
  {
    fprintf(stdout, "\tBits to hold world size: %i\n"
        "\tComm size capacity: %i\n"
        "\tComm size: %i\n"
        "\tOffset from highest rank to "
        "highest capacity for number of bits: %i\n",
        bits, 1<<bits, comm_size, (1<<bits)-comm_size);
  }
#endif

  if (reqs[0] != MPI_REQUEST_NULL)
  {
    while (reqs[num_reqs] != MPI_REQUEST_NULL) num_reqs++;
    MPI_Startall(num_reqs, reqs);
    goto cleanup;
  }

  for (i=0; i<=bits; i++)
  {
    partner_rank = rank ^ (1<<i);

    /*
     * This means the node is the lower of the partners, 
     * and will continue to the next round of gathering
     */
    if (rank < partner_rank)
    {
      if (partner_rank < comm_size)
      {
        int cnt = node_data_count(partner_rank, comm_size, recvcnts, i);
#ifndef RELEASE
        fprintf(stdout, "ISSUED RECIEVE %i <- %i (%i count at displ %i) on iter %i\n",
            rank, partner_rank, cnt, displs[partner_rank], i);
        fflush(stdout);
#endif

        if (reqs[i] == MPI_REQUEST_NULL)
        {
          MPI_Recv_init(recvbuf + displs[partner_rank], cnt, MPI_DOUBLE, 
              partner_rank, 0, comm, &reqs[i]);
        }
        MPI_Start(&reqs[i]);
      }
    }

    else
    {
      if (i > 0)
      {
#ifndef RELEASE
        fprintf(stdout, "rank %d waiting on %d recvs\n",
            rank, i);
        fflush(stdout);
#endif

        MPI_Waitall(i, reqs, MPI_STATUSES_IGNORE);

#ifndef RELEASE
        fprintf(stdout, "rank %d successfully got %d recvs\n", rank, i);
        fflush(stdout);
#endif
      }

      int cnt = node_data_count(rank, comm_size, recvcnts, i);
#ifndef RELEASE
      fprintf(stdout, "SEND %i -> %i (%i data at displ %i) on iter %i\n",
          rank, partner_rank, cnt, displs[rank], i);
      fflush(stdout);
#endif

      if (reqs[i] == MPI_REQUEST_NULL)
        MPI_Send_init(recvbuf + displs[rank], cnt,
            MPI_DOUBLE, partner_rank, 0, comm, &reqs[i]);

      MPI_Start(&reqs[i]);
      goto cleanup;
    }
  }

cleanup:
  if (num_reqs == 0)
    while (reqs[num_reqs] != MPI_REQUEST_NULL) num_reqs++;

  MPI_Waitall(num_reqs, reqs, MPI_STATUSES_IGNORE);

  if (root != 0)
  {
    if (rank == root)
      MPI_Recv(recvbuf, sum(recvcnts, comm_size-1),
          MPI_DOUBLE, 0, 0, comm, MPI_STATUS_IGNORE);

    else if (rank == 0)
      MPI_Send(recvbuf, sum(recvcnts, comm_size-1),
          MPI_DOUBLE, root, 0, comm);
  }

#ifndef RELEASE
  fprintf(stdout, "%s", RESET);
  fprintf(stdout, "EXIT: rank %d exiting gracefully.\n", rank);
  fflush(stdout);
#endif
  return 0;
}
