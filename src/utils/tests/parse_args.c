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
  assert(opts.persistent      == DEFAULT_PERSISTENT && "Default option not correctly set.");
  assert(opts.display_buf     == DEFAULT_DISPLAY_BUF && "Default option not correctly set.");
  assert(opts.num_loops       == DEFAULT_NUM_LOOPS && "Default option not correctly set.");
  assert(opts.data_per_node   == DEFAULT_DATA_PER_NODE && "Default option not correctly set.");
  assert(opts.data_per_proc   == DEFAULT_DATA_PER_PROC && "Default option not correctly set.");
  assert(opts.method          == DEFAULT_METHOD && "Default option not correctly set.");
  assert(opts.outfile         == DEFAULT_OUTFILE && "Default option not correctly set.");
  printf("%s", GREEN);
  puts("-- passed default case");

  argc=2;
  strcpy(argv[0], "bin_name");
  strcpy(argv[1], "--persist\0");
  parse_args(&argc, &argv, &opts);
  assert(opts.persistent == true);
  puts("-- passed persist flag");

  argc=2;
  strcpy(argv[0], "bin_name");
  strcpy(argv[1], "--display-buf");
  parse_args(&argc, &argv, &opts);
  assert(opts.display_buf == true);
  puts("-- passed display buffer flag");

  argc=3;
  strcpy(argv[0], "bin_name");
  strcpy(argv[1], "--output-file");
  strcpy(argv[2], "filename");
  parse_args(&argc, &argv, &opts);
  assert(opts.outfile != DEFAULT_OUTFILE);
  puts("-- passed output file flag");

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
    assert(opts.method == m);
    printf("-- passed gather method flag with %s\n", name);
  }

  argc=3;
  strcpy(argv[0], "bin_name");
  strcpy(argv[1], "--num-loops");
  strcpy(argv[2], "1");
  parse_args(&argc, &argv, &opts);
  assert(opts.num_loops == 1);
  puts("-- passed num loops flag");

  argc=3;
  strcpy(argv[0], "bin_name");
  strcpy(argv[1], "--data-per-node");
  strcpy(argv[2], "256");
  parse_args(&argc, &argv, &opts);
  assert(opts.data_per_node == 256);
  puts("-- passed data per node flag");

  for(i=0;i<argc;i++)free(argv[i]);
  free(argv);
  printf("%s", RESET);

  return 0;
}
