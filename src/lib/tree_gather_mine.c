#include "tree_gather.h"

int tree_gatherv_d(
    double * sendbuf, int sendcnt,   MPI_Datatype sendtype,
    double * recvbuf, int *recvcnts, int *displs,
    MPI_Datatype recvtype, int root, MPI_Comm comm)
{
  int i, rank, comm_size, bits=0, partner_rank;

  // only using doubles
  UNUSED(sendtype);
  UNUSED(recvtype);

  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &comm_size);

  bits = num_bits(comm_size);

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

#ifndef RELEASE
  if (rank == root)
    fprintf(stdout, "%s\tBits to hold world size: %i\n"
        "\tComm size capacity: %i\n"
        "\tComm size: %i\n"
        "\tOffset from highest rank to "
        "highest capacity for number of bits: %i\n",
        BLUE, bits, 1<<bits, comm_size, (1<<bits)-comm_size);
#endif

  for (i=0; i<=bits; i++)
  {
    partner_rank = rank ^ (1<<i);

    /*
    // I have NO idea why this works
    // used for artificial ranks for shifting root
    // around to 0 rank
    par = ar ^ (1<<i);
    if ((par^root) >= comm_size)
    {
    ar -= (1<<(bits-2));
    par = ar ^ (1<<i);
    }
    */

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
        fprintf(stdout, "RECIEVE %i <- %i (%i count at displ %i) on iter %i\n",
            rank, partner_rank, cnt, displs[partner_rank], i);
        fflush(stdout);
#endif

        MPI_Recv(recvbuf + displs[partner_rank], cnt,
            MPI_DOUBLE, partner_rank, 0, comm, MPI_STATUS_IGNORE);
      }
    }
    /*
     * The rank is the higher of the two, and will send its data to the
     * lower partner and exit. The higher rank will always have a partner -
     * only the lower of the partners has to check if the other exists
     */
    else
    {
      int cnt = node_data_count(rank, comm_size, recvcnts, i);
#ifndef RELEASE
      fprintf(stdout, "SEND %i -> %i (%i count at displ %i) on iter %i\n",
          rank, partner_rank, cnt, displs[rank], i);
      fflush(stdout);
#endif
      MPI_Send(recvbuf + displs[rank], cnt,
          MPI_DOUBLE, partner_rank, 0, comm);
      break;
    }
  }

#ifndef RELEASE
  if (rank == 0) fprintf(stdout, "%s", RESET);
#endif
  /*
   * Gave up on getting smart with the ranks - if
   * root is not 0, 0 just sends to root after finishing.
   */
  if (root != 0)
  {
    if (rank == root)
      MPI_Recv(recvbuf, sum(recvcnts, comm_size),
          MPI_DOUBLE, 0, 0, comm, MPI_STATUS_IGNORE);

    else if (rank == 0)
      MPI_Send(recvbuf, sum(recvcnts, comm_size),
          MPI_DOUBLE, root, 0, comm);
  }
  return 0;
}
