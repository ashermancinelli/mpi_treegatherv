
#include "common.h"
#include "tree_gather.h"
#include "utils.h"

#define DEFAULT_KEEP_RATIO 0.8f

/*
 * Wrapper such that the builtin gatherv matches the other
 * implementations
 */
int _MPI_Gatherv(
    double *sendbuf, int sendcnt,   MPI_Datatype sendtype,
    double *recvbuf, int *recvcnts, int *displs,
    MPI_Datatype recvtype, int root, MPI_Comm comm)
{
  return MPI_Gatherv(
      (void*)sendbuf, sendcnt, sendtype,
      (void*)recvbuf, recvcnts, displs,
      recvtype, root, comm);
}

/*
 * Function pointers to the methods that may be used
 * in the critical loop
 */
int (*gatherv_function)(
    double *sendbuf, int sendcnt,   MPI_Datatype sendtype,
    double *recvbuf, int *recvcnts, int *displs,
    MPI_Datatype recvtype, int root, MPI_Comm comm);
int (*persistent_gatherv_function)(
    double *sendbuf, int sendcnt, MPI_Datatype sendtype,
    double *recvbuf, int *recvcnts, int *displs,
    MPI_Datatype recvtype, int root, MPI_Comm comm,
    MPI_Request* reqs);

int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);
  struct options opts;
  parse_args(&argc, &argv, &opts);
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Comm_size(MPI_COMM_WORLD, &opts.size);
  MPI_Comm_rank(MPI_COMM_WORLD, &opts.rank);
  int i, j;
  bool display_buf = false;
  bool persistent = false;
  double *global_buffer;
  double *local_buffer;
  MPI_Request reqs[MAX_MPI_RANKS];

  int *offsets = malloc(sizeof(int) * opts.size);
  int *cnts = malloc(sizeof(int) * opts.size);
  opts.outfile = stdout;

  global_buffer = malloc(sizeof(double) * opts.size * opts.data_per_node);
  local_buffer = malloc(sizeof(double) * opts.data_per_node);

  offsets[0] = 0;
  for (i=0; i<opts.size; i++)
    cnts[i] = opts.data_per_node;
  for (j=1; j < opts.size; j++)
    offsets[j] = offsets[j-1] + cnts[j-1];
  for (j=0; j<cnts[opts.rank]; j++)
    local_buffer[j] = (double)(opts.rank + 1);
  for (i=0; i<MAX_MPI_RANKS; i++) reqs[i] = MPI_REQUEST_NULL;

  if (!opts.rank)
  {
    if (display_buf)
    {
      fprintf(opts.outfile, "Config:\n");
      for (i=0; i<opts.size; i++)
        fprintf(opts.outfile, "cnts[%i] = %i\n", i, cnts[i]);
      fprintf(opts.outfile, "\n");

      for (i=0; i<opts.size; i++)
        fprintf(opts.outfile, "offset[%i] = %i\n", i, offsets[i]);
      fprintf(opts.outfile, "\n");

      for (j=0; j<cnts[opts.rank]; j++)
        fprintf(opts.outfile, "local_buffer[%i] = %.1f\n", j, local_buffer[j]);
      fprintf(opts.outfile, "\n");
    }
    fprintf(opts.outfile, "Using gather method: %s.\n", method_to_str(opts.method));
    fprintf(opts.outfile, "Looping %d times.\n", opts.num_loops);
    fprintf(opts.outfile, "Using %d data points per node.\n", opts.data_per_node);
    fprintf(opts.outfile, "Using %d procs.\n\n", opts.size);
    fflush(opts.outfile);
  }

  /*
   * Set the function pointers according to the
   * method passed via CLI
   */
  if (opts.method == MPI)
    gatherv_function = &_MPI_Gatherv;
  else if (opts.method == TREE)
    gatherv_function = &tree_gatherv_d;
  else if (opts.method == ITREE && !persistent)
    gatherv_function = &tree_gatherv_d_async;
  else if (opts.method == ITREE && persistent)
    persistent_gatherv_function = &tree_gatherv_d_persistent;
  else if (opts.method == MYMPI && !persistent)
    gatherv_function = &my_mpi_gatherv;
  else if (opts.method == MYMPI && persistent)
    persistent_gatherv_function = &my_mpi_gatherv_persistent;

  fflush(opts.outfile);

  // critical loop
  if (persistent)
  {
    if (!opts.rank)
      fprintf(stdout, "Running critical loop with "
          "persistent communication.\n");
    for (i=0; i<opts.num_loops; i++)
    {
#ifdef __DEBUG
      for (j=0; j<cnts[opts.rank]; j++)
        fprintf(opts.outfile, "MAIN.C RANK(%i) local_buffer[%i] = %.1f\n",
            opts.rank, j, local_buffer[j]);
      fprintf(opts.outfile, "\n");
#endif
      assert(MPI_Barrier(MPI_COMM_WORLD)==MPI_SUCCESS);
      (*persistent_gatherv_function)(
          local_buffer,
          cnts[opts.rank],
          MPI_DOUBLE,
          global_buffer,
          cnts,
          offsets,
          MPI_DOUBLE,
          0,
          MPI_COMM_WORLD,
          reqs);
      assert(MPI_Barrier(MPI_COMM_WORLD)==MPI_SUCCESS);
    }
  }
  else
  {
    if (!opts.rank)
      fprintf(stdout, "Running critical loop with "
          "non-persistent communication.\n");
    for (i=0; i<opts.num_loops; i++)
    {
#ifdef __DEBUG
      for (j=0; j<cnts[opts.rank]; j++)
        fprintf(opts.outfile, "MAIN.C RANK(%i) local_buffer[%i] = %.1f\n",
            opts.rank, j, local_buffer[j]);
      fprintf(opts.outfile, "\n");
#endif
      assert(MPI_Barrier(MPI_COMM_WORLD)==MPI_SUCCESS);
      (*gatherv_function)(
          local_buffer,
          cnts[opts.rank],
          MPI_DOUBLE,
          global_buffer,
          cnts,
          offsets,
          MPI_DOUBLE,
          0,
          MPI_COMM_WORLD);
      assert(MPI_Barrier(MPI_COMM_WORLD)==MPI_SUCCESS);
    }
  }
  // \critical loop

  if (!opts.rank && opts.display_buf)
  {
    fprintf(opts.outfile, "Global buffer: \n");
    for (i=0; i<opts.data_per_node*opts.size; i++)
      fprintf(opts.outfile, "global_buffer[%i] = %.2f\n", i, global_buffer[i]);
    fprintf(opts.outfile, "\n");
  }

  free(global_buffer);free(local_buffer);

  if (opts.outfile != stdout)
    fclose(opts.outfile);

  MPI_Finalize();
  return 0;
}
