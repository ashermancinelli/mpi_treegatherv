#include "treegatherv.h"

/*
 * Using async recieves. If a sender has pending
 * recvs, wait on all recieves before sending and dying
 */
int tree_async_gatherv(
    double *sendbuf, int sendcnt, MPI_Datatype sendtype,
    double *recvbuf, int *recvcnts, int *displs,
    MPI_Datatype recvtype, int root, MPI_Comm comm)
{
  int i, rank, comm_size, bits=0, partner_rank;

  UNUSED(sendtype);
  UNUSED(recvtype);

  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &comm_size);

#if !defined RELEASE && VERBOSE_OUTPUT
  for (i=0; i<recvcnts[rank]; i++)
    fprintf(stdout, "TREE_GATHER.C RANK(%i) local_buffer[%i] = %.1f\n",
        rank, i, sendbuf[i]);
#endif

  /*
   * Copy your local bit into the global buf
   * unless you are root and you pass in a null
   * to indicate you are using global recv buff as
   * your send buf to use in-place buf
   */
  if (recvbuf == NULL)
    recvbuf = sendbuf;
  else
    memcpy(recvbuf + displs[rank], sendbuf, sizeof(double)*sendcnt);

  bits = num_bits(comm_size);

  /*
   * Max number of async recvs that you could have is the 
   * number of possible merges, aka number of bits to hold
   * world comm size
   */
  // MPI_Request *rec_hdls = malloc(sizeof(MPI_Request) * bits);
  // I think the above is causing problems. Statically allocating
  // is annoying but hopefully it prevents the memory corruption
  MPI_Request rec_hdls[MAX_MPI_BITS];

#if !defined RELEASE && VERBOSE_OUTPUT
  fprintf(stdout, "%s", BLUE);
  if (rank == root)
    fprintf(stdout, "\tBits to hold world size: %i\n"
        "\tComm size capacity: %i\n"
        "\tComm size: %i\n"
        "\tOffset from highest rank to "
        "highest capacity for number of bits: %i\n",
        bits, 1<<bits, comm_size, (1<<bits)-comm_size);
#endif

  for (i=0; i<=bits; i++)
  {
    partner_rank = rank ^ (1<<i);
    rec_hdls[i] = MPI_REQUEST_NULL;

    /*
     * This means the node is the lower of the partners, 
     * and will continue to the next round of gathering
     */
    if (rank < partner_rank)
    {
      if (partner_rank < comm_size)
      {
        int cnt = node_data_count(partner_rank, comm_size, recvcnts, i);
#if !defined RELEASE && VERBOSE_OUTPUT
        fprintf(stdout, "ISSUED RECIEVE %i <- %i (%i count at displ %i) on iter %i\n",
            rank, partner_rank, cnt, displs[partner_rank], i);
        fflush(stdout);
#endif

        MPI_Irecv(recvbuf + displs[partner_rank], cnt, MPI_DOUBLE, 
            partner_rank, 0, comm, &rec_hdls[i]);
      }
    }

    /*
     * The rank is the higher of the two, and will send its data to the
     * lower partner and exit. The higher rank will always have a partner -
     * only the lower of the partners has to check if the other exists
     */
    else
    {
      /*
       * Wait for any pending recv's to arrive. If there are none,
       * move on to the send and die.
       */
      if (i > 0)
      {
#if !defined RELEASE && VERBOSE_OUTPUT
        fprintf(stdout, "rank %d waiting on %d recvs\n",
            rank, i);
        fflush(stdout);
#endif

        MPI_Waitall(i, rec_hdls, MPI_STATUSES_IGNORE);

#if !defined RELEASE && VERBOSE_OUTPUT
        fprintf(stdout, "rank %d successfully got %d recvs\n", rank, i);
        fflush(stdout);
#endif
      }

      int cnt = node_data_count(rank, comm_size, recvcnts, i);
#if !defined RELEASE && VERBOSE_OUTPUT
      fprintf(stdout, "SEND %i -> %i (%i data at displ %i) on iter %i\n",
          rank, partner_rank, cnt, displs[rank], i);
      fflush(stdout);
#endif

      MPI_Send(recvbuf + displs[rank], cnt,
          MPI_DOUBLE, partner_rank, 0, comm);

      /*
       * After sending data from node and all valid children,
       * node will never have another recv and can die.
       */
      return 0;
    }
  }

  /*
   * Only root will get here - all other
   * nodes will send and die eventually and root
   * will be the last one alive.
   */

#if !defined RELEASE && VERBOSE_OUTPUT
  if (rank == root)
  {
    fprintf(stdout, "Root node waiting on %d"
        " issued recieves.\n", bits);
    fflush(stdout);
  }
#endif

  if (rank == 0)
  {
    // number of bits == number of merges, so
    // root will have to wait on that many recvs
    MPI_Waitall(bits, rec_hdls, MPI_STATUSES_IGNORE);
    int total = sum(recvcnts, comm_size);
#if !defined RELEASE && VERBOSE_OUTPUT
    for (i=0; i<total; i++)
      fprintf(stdout, "TREE_GATHER.C RANK(%i) global_buffer[%i] = %.1f\n",
          rank, i, recvbuf[i]);
#else
    (void)total;
#endif
  }

  /*
   * Gave up on getting smart with the ranks - if
   * root is not 0, 0 just sends to root after finishing.
   */
  if (root != 0)
  {
    if (rank == root)
    {
      MPI_Recv(recvbuf, sum(recvcnts, comm_size-1),
          MPI_DOUBLE, 0, 0, comm, MPI_STATUS_IGNORE);
#if !defined RELEASE && VERBOSE_OUTPUT
      fprintf(stdout, "Root node exiting gracefully.\n");
      fflush(stdout);
#endif
    }
    else if (rank == 0)
      MPI_Send(recvbuf, sum(recvcnts, comm_size-1),
          MPI_DOUBLE, root, 0, comm);
  }

#if !defined RELEASE && VERBOSE_OUTPUT
  fprintf(stdout, "%s", RESET);
  fprintf(stdout, "EXIT: rank %d exiting gracefully.\n", rank);
  fflush(stdout);
#endif

  return 0;
}
