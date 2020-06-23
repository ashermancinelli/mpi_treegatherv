#include "utils.h"

int main(int argc, char** argv)
{
  struct options opts;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &opts.rank);
  MPI_Comm_size(MPI_COMM_WORLD, &opts.size);
  set_default_opts(&opts);
  assert(opts.persistent      == DEFAULT_PERSISTENT && "Default option not correctly set.");
  assert(opts.display_buf     == DEFAULT_DISPLAY_BUF && "Default option not correctly set.");
  assert(opts.num_loops       == DEFAULT_NUM_LOOPS && "Default option not correctly set.");
  assert(opts.data_per_node   == DEFAULT_DATA_PER_NODE && "Default option not correctly set.");
  assert(opts.data_per_proc   == DEFAULT_DATA_PER_PROC && "Default option not correctly set.");
  assert(opts.method          == DEFAULT_METHOD && "Default option not correctly set.");
  assert(opts.outfile         == DEFAULT_OUTFILE && "Default option not correctly set.");
  printf("%s", GREEN);
  puts("-- passed set_default_opts");

  int cnts[] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
  int iter;
  int size;
  int rank;
  size_t cnt;

  size = 16; // length of cnts
  iter = 0; // iteration we're testing
  rank = 0; // rank we're testing

  cnt = node_data_count(rank, size, cnts, iter);
  assert(cnt == 2);
  printf("-- passed node_data_count iter %d rank %d\n", iter, rank);

  iter++;
  cnt = node_data_count(rank, size, cnts, iter);
  assert(cnt == 2*2);
  printf("-- passed node_data_count iter %d rank %d\n", iter, rank);

  iter++;
  cnt = node_data_count(rank, size, cnts, iter);
  assert(cnt == 2*2*2);
  printf("-- passed node_data_count iter %d rank %d\n", iter, rank);

  iter++;
  cnt = node_data_count(rank, size, cnts, iter);
  assert(cnt == 2*2*2*2);
  printf("-- passed node_data_count iter %d rank %d\n", iter, rank);

  iter = 0;
  rank = 8;
  cnt = node_data_count(rank, size, cnts, iter);
  assert(cnt == 2);
  printf("-- passed node_data_count iter %d rank %d\n", iter, rank);

  iter++;
  cnt = node_data_count(rank, size, cnts, iter);
  assert(cnt == 2*2);
  printf("-- passed node_data_count iter %d rank %d\n", iter, rank);

  iter++;
  cnt = node_data_count(rank, size, cnts, iter);
  assert(cnt == 2*2*2);
  printf("-- passed node_data_count iter %d rank %d\n", iter, rank);

  iter++;
  cnt = node_data_count(rank, size, cnts, iter);
  assert(cnt == 2*2*2*2);
  printf("-- passed node_data_count iter %d rank %d\n", iter, rank);
  
  printf("%s", RESET);
  return 0;
}
