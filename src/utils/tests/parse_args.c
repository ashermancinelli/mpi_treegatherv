#include "utils.h"

#define MAX_NARGS 15

int main(int _argc, char** _argv)
{
  MPI_Init(&_argc, &_argv);
  int argc;
  int i;
  char** argv;
  struct options opts;
  MPI_Comm_rank(MPI_COMM_WORLD, &opts.rank);
  MPI_Comm_size(MPI_COMM_WORLD, &opts.size);

  // Default case
  argc=1; argv = malloc(sizeof(char*)*MAX_NARGS);
  for(i=0; i<MAX_NARGS; i++) argv[i] = malloc(BUFSIZ);
  parse_args(&argc, &argv, &opts);
  TESTCASE(opts.persistent      == DEFAULT_PERSISTENT   , "DEFAULT_PERSISTENT   ");
  TESTCASE(opts.display_buf     == DEFAULT_DISPLAY_BUF  , "DEFAULT_DISPLAY_BUF  ");
  TESTCASE(opts.num_loops       == DEFAULT_NUM_LOOPS    , "DEFAULT_NUM_LOOPS    ");
  TESTCASE(opts.data_per_node   == DEFAULT_DATA_PER_NODE, "DEFAULT_DATA_PER_NODE");
  TESTCASE(opts.data_per_proc   == DEFAULT_DATA_PER_PROC, "DEFAULT_DATA_PER_PROC");
  TESTCASE(opts.method          == DEFAULT_METHOD       , "DEFAULT_METHOD       ");
  TESTCASE(opts.outfile         == DEFAULT_OUTFILE      , "DEFAULT_OUTFILE      ");

  // Must use another gather method since there is no
  // persistent version of the default MPI algorithm
  argc=4;
  strcpy(argv[0], "bin_name");
  strcpy(argv[1], "--persist");
  strcpy(argv[2], "--gather-method");
  strcpy(argv[3], "itree");
  parse_args(&argc, &argv, &opts);
  TESTCASE(opts.persistent, "persist flag");

  argc=2;
  strcpy(argv[0], "bin_name");
  strcpy(argv[1], "--display-buf");
  parse_args(&argc, &argv, &opts);
  TESTCASE(opts.display_buf, "display buffer flag");

  argc=3;
  strcpy(argv[0], "bin_name");
  strcpy(argv[1], "--output-file");
  strcpy(argv[2], "filename");
  parse_args(&argc, &argv, &opts);
  TESTCASE(opts.outfile!=DEFAULT_OUTFILE, "output file flag");

  enum gather_method methods[] = { MPI, MYMPI, TREE, ITREE };
  char* method_names[] = { "mpi", "my-mpi", "tree", "itree" };
  for(i=0; i<4; i++)
  {
    enum gather_method m = methods[i];
    char* name = strdup(method_names[i]);

    argc=3;
    strcpy(argv[0], "bin_name");
    strcpy(argv[1], "--gather-method");
    strcpy(argv[2], name);
    parse_args(&argc, &argv, &opts);
    TESTCASE(opts.method==m, name);
  }

  argc=3;
  strcpy(argv[0], "bin_name");
  strcpy(argv[1], "--num-loops");
  strcpy(argv[2], "1");
  parse_args(&argc, &argv, &opts);
  TESTCASE(opts.num_loops == 1, "num loops flag");

  argc=3;
  strcpy(argv[0], "bin_name");
  strcpy(argv[1], "--data-per-node");
  strcpy(argv[2], "256");
  parse_args(&argc, &argv, &opts);
  TESTCASE(opts.data_per_node == 256, "data per node flag");

  for(i=0;i<argc;i++)free(argv[i]);
  free(argv);
  printf("%s", RESET);

  return 0;
}
