#include "utils.h"

int main(int argc, char** argv)
{
  struct options opts;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &opts.rank);
  MPI_Comm_size(MPI_COMM_WORLD, &opts.size);
  set_default_opts(&opts);
  TESTCASE(opts.persistent      == DEFAULT_PERSISTENT   , "DEFAULT_PERSISTENT   ");
  TESTCASE(opts.display_buf     == DEFAULT_DISPLAY_BUF  , "DEFAULT_DISPLAY_BUF  ");
  TESTCASE(opts.num_loops       == DEFAULT_NUM_LOOPS    , "DEFAULT_NUM_LOOPS    ");
  TESTCASE(opts.data_per_node   == DEFAULT_DATA_PER_NODE, "DEFAULT_DATA_PER_NODE");
  TESTCASE(opts.data_per_proc   == DEFAULT_DATA_PER_PROC, "DEFAULT_DATA_PER_PROC");
  TESTCASE(opts.method          == DEFAULT_METHOD       , "DEFAULT_METHOD       ");
  TESTCASE(opts.outfile         == DEFAULT_OUTFILE      , "DEFAULT_OUTFILE      ");

  int cnts[] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
  int iter;
  int size;
  int rank;
  size_t cnt;
  char buf[BUFSIZ];

  size = 16; // length of cnts
  iter = 0; // iteration we're testing
  rank = 0; // rank we're testing

  printf("node_data_count tests\n");

  cnt = node_data_count(rank, size, cnts, iter);
  sprintf(buf, "iter %d rank %d", iter, rank);
  TESTCASE(cnt == 2, buf);

  iter++;
  cnt = node_data_count(rank, size, cnts, iter);
  sprintf(buf, "iter %d rank %d", iter, rank);
  TESTCASE(cnt == 2*2, buf);

  iter++;
  cnt = node_data_count(rank, size, cnts, iter);
  sprintf(buf, "iter %d rank %d", iter, rank);
  TESTCASE(cnt == 2*2*2, buf);

  iter++;
  cnt = node_data_count(rank, size, cnts, iter);
  sprintf(buf, "iter %d rank %d", iter, rank);
  TESTCASE(cnt == 2*2*2*2, buf);

  iter = 0;
  rank = 8;
  cnt = node_data_count(rank, size, cnts, iter);
  sprintf(buf, "iter %d rank %d", iter, rank);
  TESTCASE(cnt == 2, buf);

  iter++;
  cnt = node_data_count(rank, size, cnts, iter);
  sprintf(buf, "iter %d rank %d", iter, rank);
  TESTCASE(cnt == 2*2, buf);

  iter++;
  cnt = node_data_count(rank, size, cnts, iter);
  sprintf(buf, "iter %d rank %d", iter, rank);
  TESTCASE(cnt == 2*2*2, buf);

  iter++;
  cnt = node_data_count(rank, size, cnts, iter);
  sprintf(buf, "iter %d rank %d", iter, rank);
  TESTCASE(cnt == 2*2*2*2, buf);
  
  return 0;
}
