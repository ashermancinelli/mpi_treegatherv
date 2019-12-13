
#include "tree_gather.h"

static inline __attribute__((always_inline))
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

static inline __attribute__((always_inline))
unsigned int sum(int* ar, int count)
{
    int n = 0, i;
    for (i=0; i<count; i++)
        n += ar[i];
    return n;
}

/*
 * Gets the offset for a node plus the offsets of it's
 * children
 */
size_t node_data_count(
        int rank,
        int size,
        int* cnts,
        int iteration)
{
    size_t count = 0;
    int i, rightmost_child=0;
    for (i=iteration; i>0; i--)
    {
        rightmost_child <<= 1;
        rightmost_child++;
    }
    rightmost_child += rank;

    for (i=rank; i<=rightmost_child; i++)
    {
        if (i >= size)
            return count;
        count += cnts[i];
    }
    return count;
}

inline int min_child_rank(
        int* children,
        int len)
{
    int i, min=99999;
    for (i=0; i<len; i++)
        min = children[i] < min ? children[i] : min;
    return min;
}

inline int max_child_rank(
        int* children,
        int len)
{
    int i, min=0;
    for (i=0; i<len; i++)
        min = children[i] > min ? children[i] : min;
    return min;
}

/*
 * Only using floats for now.
 *
 * See header for algorithm explanation.
 * Using this for reference: http://formalverification.cs.utah.edu/sawaya/html/d0/d70/mpi_2coll_2gather_8c-source.html
 *
 * Only gather with root = 0
 */
void tree_gatherv_d(
        double * restrict sendbuf, int sendcnt,   MPI_Datatype sendtype,
        double * restrict recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm)
{
    int i, rank, comm_size, bits=0, partner_rank;

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
    {
        recvbuf = sendbuf;
    }
    else
    {
        memcpy(recvbuf + displs[rank], sendbuf, sizeof(double)*sendcnt);
    }

#   ifdef __DEBUG
        if (rank == root)
        {
            fprintf(stdout, "\tBits to hold world size: %i\n"
                "\tComm size capacity: %i\n"
                "\tComm size: %i\n"
                "\tOffset from highest rank to "
                "highest capacity for number of bits: %i\n",
                bits, 1<<bits, comm_size, (1<<bits)-comm_size);
        }
#   endif

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
#               ifdef __DEBUG
                    fprintf(stdout, "RECIEVE %i <- %i (%i count at displ %i) on iter %i\n",
                            rank, partner_rank, cnt, displs[partner_rank], i);
                    fflush(stdout);
#               endif

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
#           ifdef __DEBUG
                fprintf(stdout, "SEND %i -> %i (%i data at displ %i) on iter %i\n",
                        rank, partner_rank, cnt, displs[rank], i);
                fflush(stdout);
#           endif
            MPI_Send(recvbuf + displs[rank], cnt,
                MPI_DOUBLE, partner_rank, 0, comm);
            break;
        }
    }

    /*
     * Gave up on getting smart with the ranks - if
     * root is not 0, 0 just sends to root after finishing.
     */
    if (root != 0)
    {
        if (rank == root)
        {
            MPI_Recv(recvbuf, sum(recvcnts, comm_size),
                    MPI_DOUBLE, 0, 0, comm, MPI_STATUS_IGNORE);
        }
        else if (rank == 0)
        {
            MPI_Send(recvbuf, sum(recvcnts, comm_size),
                    MPI_DOUBLE, root, 0, comm);
        }
        else
        {
            return;
        }
    }
}

/*
 * Using async recieves. If a sender has pending
 * recvs, wait on all recieves before sending and dying
 */
void tree_gatherv_d_async(
        double *sendbuf, int sendcnt, MPI_Datatype sendtype,
        double *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm)
{
    int i, rank, comm_size, bits=0, partner_rank;

    assert(root == 0);

    UNUSED(sendtype);
    UNUSED(recvtype);

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &comm_size);

    /*
     * Copy your local bit into the global buf
     * unless you are root and you pass in a null
     * to indicate you are using global recv buff as
     * your send buf to use in-place buf
     */
    if (recvbuf == NULL)
    {
        recvbuf = sendbuf;
    }
    else
    {
        memcpy(recvbuf + displs[rank], sendbuf, sizeof(double)*sendcnt);
    }

    bits = num_bits(comm_size);

    /*
     * Max number of async recvs that you could have is the 
     * number of possible merges, aka number of bits to hold
     * world comm size
     */
    MPI_Request *rec_hdls = malloc(sizeof(MPI_Request) * bits);

#   ifdef __DEBUG
        if (rank == root)
        {
            fprintf(stdout, "\tBits to hold world size: %i\n"
                "\tComm size capacity: %i\n"
                "\tComm size: %i\n"
                "\tOffset from highest rank to "
                "highest capacity for number of bits: %i\n",
                bits, 1<<bits, comm_size, (1<<bits)-comm_size);
        }
#   endif

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
#               ifdef __DEBUG
                    fprintf(stdout, "ISSUED RECIEVE %i <- %i (%i count at displ %i) on iter %i\n",
                            rank, partner_rank, cnt, displs[partner_rank], i);
                    fflush(stdout);
#               endif

                MPI_Irecv(recvbuf + displs[partner_rank], cnt, MPI_DOUBLE, 
                        partner_rank, 0, comm, 
                        &rec_hdls[i]);
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
#               ifdef __DEBUG
                    fprintf(stdout, "rank %d waiting on %d recvs\n",
                            rank, i);
                    fflush(stdout);
#               endif

                MPI_Waitall(i, rec_hdls, MPI_STATUSES_IGNORE);

#               ifdef __DEBUG
                    fprintf(stdout, "rank %d successfully got %d recvs\n", rank, i);
                    fflush(stdout);
#               endif
            }

            int cnt = node_data_count(rank, comm_size, recvcnts, i);
#           ifdef __DEBUG
                fprintf(stdout, "SEND %i -> %i (%i data at displ %i) on iter %i\n",
                        rank, partner_rank, cnt, displs[rank], i);
                fflush(stdout);
#           endif

            MPI_Send(recvbuf + displs[rank], cnt,
                MPI_DOUBLE, partner_rank, 0, comm);

            /*
             * After sending data from node and all valid children,
             * node will never have another recv and can die.
             */
            return;
        }
    }

    /*
     * Only root will get here - all other
     * nodes will send and die eventually and root
     * will be the last one alive.
     */

#   ifdef __DEBUG
        if (rank == root)
        {
            fprintf(stdout, "Root node waiting on %d"
                    " issued recieves.\n", bits);
            fflush(stdout);
        }
#   endif

    if (rank == 0)
    {
        // number of bits == number of merges, so
        // root will have to wait on that many recvs
        MPI_Waitall(bits, rec_hdls, MPI_STATUSES_IGNORE);
    }

    /*
     * Gave up on getting smart with the ranks - if
     * root is not 0, 0 just sends to root after finishing.
     */
    /*
    if (root != 0)
    {
        if (rank == root)
        {
            MPI_Recv(recvbuf, sum(recvcnts, comm_size-1),
                    MPI_DOUBLE, 0, 0, comm, MPI_STATUS_IGNORE);
#           ifdef __DEBUG
                fprintf(stdout, "Root node exiting gracefully.\n");
                fflush(stdout);
#           endif
        }
        else if (rank == 0)
        {
            MPI_Send(recvbuf, sum(recvcnts, comm_size-1),
                    MPI_DOUBLE, root, 0, comm);
        }
    }
    */

#   ifdef __DEBUG
        fprintf(stdout, "EXIT: rank %d exiting gracefully.\n", rank);
        fflush(stdout);
#   endif

}

void my_mpi_gatherv(
        double *sendbuf, int sendcnt,   MPI_Datatype sendtype,
        double *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm)
{
    int i, rank, comm_size;

    UNUSED(recvtype);
    UNUSED(sendtype);

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &comm_size);

    /*
     * Algorithm for normal gatherv
     * 
     * Note that even root sends to itself in this
     * algorithm
     */
    for (i=0; i<comm_size; i++)
    {
        MPI_Send(sendbuf, sendcnt, MPI_DOUBLE, root, 0, comm);
    }

    if (rank == root)
    {
        for (i=0; i<comm_size; i++)
        {
            MPI_Recv(recvbuf + displs[i], recvcnts[i], MPI_DOUBLE, i, 0, comm, MPI_STATUS_IGNORE);
        }
    }
}
