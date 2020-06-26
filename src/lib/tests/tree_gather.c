#include "treegatherv.h"

#define data_per_node 2048

/*
 * Check the output of each gatherv function
 */
void err_chck(double* recvbuf, int* recvcnts, int* displs, int root, MPI_Comm comm)
{
  int size, rank, bufsz, i;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  // All ranks should now be able to check their values
  bufsz=0; for(i=0;i<size;i++) bufsz+=recvcnts[i];
  i=MPI_Bcast(recvbuf, bufsz, MPI_DOUBLE, root, comm);
  assert(i==MPI_SUCCESS);
  MPI_Barrier(comm);

  for(i=displs[rank]; i<recvcnts[rank]; i++)
    assert(recvbuf[i]==(double)(rank+1));
}

int main(int argc, char** argv)
{
  int ierr, size, rank, i, j;
  MPI_Request reqs[MAX_MPI_RANKS];
  MPI_Comm comm = MPI_COMM_WORLD;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);
  MPI_Barrier(comm);
  int *offsets = malloc(sizeof(int) * size);
  int *cnts = malloc(sizeof(int) * size);
  double* global_buffer = malloc(sizeof(double) * size * data_per_node);
  double* local_buffer = malloc(sizeof(double) * data_per_node);

  offsets[0] = 0;
  for (i=0; i<size; i++)
    cnts[i] = data_per_node;
  for (j=1; j < size; j++)
    offsets[j] = offsets[j-1] + cnts[j-1];
  for (j=0; j<cnts[rank]; j++)
    local_buffer[j] = (double)(rank + 1);
  for (i=0; i<MAX_MPI_RANKS; i++) reqs[i] = MPI_REQUEST_NULL;

  ierr = tree_gatherv(
      local_buffer, cnts[rank], MPI_DOUBLE,
      global_buffer, cnts, offsets, MPI_DOUBLE,
      0, comm);
  assert(ierr == 0);
  err_chck(global_buffer, cnts, offsets, 0, comm);
  if(rank==0) TESTCASE(true, "tree");

  ierr = tree_async_gatherv(
      local_buffer, cnts[rank], MPI_DOUBLE,
      global_buffer, cnts, offsets, MPI_DOUBLE,
      0, comm);
  assert(ierr == 0);
  err_chck(global_buffer, cnts, offsets, 0, comm);
  if(rank==0) TESTCASE(true, "tree async");

  ierr = tree_async_persistent_gatherv(
      local_buffer, cnts[rank], MPI_DOUBLE,
      global_buffer, cnts, offsets, MPI_DOUBLE,
      0, comm, reqs);
  assert(ierr == 0);
  err_chck(global_buffer, cnts, offsets, 0, comm);
  if(rank==0) TESTCASE(true, "tree async persistent");

  ierr = mpi_gatherv(
      local_buffer, cnts[rank], MPI_DOUBLE, 
      global_buffer, cnts, offsets, MPI_DOUBLE, 
      0, comm);
  assert(ierr == 0);
  err_chck(global_buffer, cnts, offsets, 0, comm);
  if(rank==0) TESTCASE(true, "mpi");

  ierr = mpi_async_persistent_gatherv(
      local_buffer, cnts[rank], MPI_DOUBLE, 
      global_buffer, cnts, offsets, MPI_DOUBLE, 
      0, comm, reqs);
  assert(ierr == 0);
  err_chck(global_buffer, cnts, offsets, 0, comm);
  if(rank==0) TESTCASE(true, "mpi async persistent");

  free(local_buffer); free(global_buffer);
  MPI_Finalize();
  return 0;
}
